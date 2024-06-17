#include "route_server.hpp"
#include "../base/listener.hpp"
#include "auth_session.hpp"
#include "route_session.hpp"
#include "business_session.hpp"
#include "supervisor_session.hpp"

#include "../tools/proto/server_message.pb.h"
#include "../tools/proto/supervisor_message.pb.h"

#include <format>

namespace leo {
namespace route {
;
route_server::route_server(net::io_context& ioc)
	: server{ ioc }
	, business_lb_{ ioc }
	, route_info_channel_{ ioc, 4096 }
	, node_info_to_supervisor_{ ioc, 4096 }
	, node_info_to_route_{ ioc, 4096 }
{
	net::co_spawn(
		ioc, 
		route_info_distributor(), 
		net::bind_cancellation_slot(signals().slot(), net::detached)
	);

	net::co_spawn(
		ioc,
		node_info_distributor(),
		net::bind_cancellation_slot(signals().slot(), net::detached)
	);
}

void route_server::start_impl() {
	auto listener		= std::make_shared<leo::listener>(ioc_, ctx_);
	auth_port_			= listener->run<route_server, auth_session>(*this, "route_auth_port_range");
	route_port_			= listener->run<route_server, route_session>(*this, "route_route_port_range");
	business_port_		= listener->run<route_server, business_session>(*this, "route_business_port_range");
	supervisor_port_	= listener->run<route_server, supervisor_session>(*this, "route_supervisor_port_range");

	// signup services
	std::string host{ config_loader::load_config()["host"].get<std::string>() };
	if (auth_port_) {
		cache_.signup_service(table_auth_list, std::format("{}:{}", host, auth_port_));
		log().info("listening autn_server on port: {}", auth_port_);
	}
	if (business_port_) {
		cache_.signup_service(table_business_list, std::format("{}:{}", host, business_port_));
		log().info("listening business_server on port: {}", business_port_);
	}
	if (route_port_) {
		cache_.signup_service(table_route_list, std::format("{}:{}", host, route_port_));
		log().info("listening route_server on port: {}", route_port_);
		set_uri(std::format("{}:{}", host, route_port_));

		connect_route();
	}
}

void route_server::connect_route() {
	bool find_route{ false };
	boost::system::error_code ec;
	auto host = config_loader::load_config()["host"].get<std::string>();
	auto route_list = cache_.get_services(table_route_list);
	for (auto& uri : route_list) {
		if (uri == uri_) {
			// skip self
			continue;
		}

		auto route = make_route_session(uri);
		if (!route) {
			log().error("connect_route failed: {} code: {}", ec.message(), ec.value());
			continue;
		}

		route->start();
		log().info("connect to route: {}", uri);

		message_type::route_route msg;
		msg.set_category(message_type::SERVER_INFO);
		msg.set_uri(uri_);
		route->deliver(msg.SerializeAsString());

		find_route = true;
	}

	if (!find_route) {
		log().info("no another route server found");
	}
}

void route_server::stop_impl() {
	std::string host{ config_loader::load_config()["host"].get<std::string>() };
	if (auth_port_) {
		cache_.remove_service(table_auth_list, std::format("{}:{}", host, auth_port_));
	}
	if (route_port_) {
		cache_.remove_service(table_route_list, std::format("{}:{}", host, route_port_));
	}
	if (business_port_) {
		cache_.remove_service(table_business_list, std::format("{}:{}", host, business_port_));
	}
	if (supervisor_port_) {
		cache_.remove_service(table_supervisor_list, std::format("{}:{}", host, supervisor_port_));
	}

	// clear all the sessions
	auth_list_.clear(); 
	auth_temp_.clear();
	route_list_.clear();
	route_temp_.clear();
	business_list_.clear(); 
	business_temp_.clear();
	supervisor_list_.clear();
	supervisor_temp_.clear();
	business_lb_.stop();

}

void route_server::store_impl() {
}

route_ptr route_server::make_route_session(const std::string& uri) {
	boost::system::error_code ec;
	auto [host, port] = uri2host_port(uri);
	tcp::resolver resolver{ ioc_ };
	auto endpoints = resolver.resolve(host, port);
	tcp::socket socket{ ioc_ };
	net::connect(socket, endpoints, ec);
	if (ec) {
		return nullptr;
	}

	auto route = std::make_shared<route_session>(
			beast::ssl_stream<beast::tcp_stream>{
				beast::tcp_stream{ std::move(socket) },
				ctx_
			},
			* this
		);

	route->as_client();
	route->set_local_uri(uri_);

	// 将新建的route_session加入发起连接的列表
	perm_add(uri, route);
	return route;
}

net::awaitable<void> route_server::route_info_distributor() {
	error_code ec;
	auto token = net::redirect_error(net::deferred, ec);
	std::string message;
	message.reserve(1024);
	while (true) {
		message = co_await route_info_channel_.async_receive(token);
		if (ec && ec != net::error::operation_aborted && ec != expr::error::channel_cancelled) {
			log().error("route_info_distributor: {}, code: {}, name: {}", ec.message(), ec.value(), ec.category().name());
			break;
		}

		for (auto& [key, session] : auth_list_) {
			//std::println("auth_list_ session: {}", session->remote_uri());
			session->deliver(message);
		}
	}
}

net::awaitable<void> route_server::push_route_info(const std::string& message) {
	error_code ec;
	auto token = net::redirect_error(net::deferred, ec);
	co_await route_info_channel_.async_send({}, message, token);
	if (ec && ec != net::error::operation_aborted) {
		log().error("push_route_info: {}, code: {}, name: {}", ec.message(), ec.value(), ec.category().name());
	}
}

net::awaitable<void> route_server::node_info_distributor() {
	error_code ec;
	net::steady_timer timer{ ioc_ };
	std::atomic<bool> timer_flag{ true };
	auto token = net::redirect_error(net::deferred, ec);
	auto interval = std::chrono::milliseconds(config_loader::load_config()["route_update_interval"].get<int>());
	std::string buffer;
	message_type::route_route msg_route;
	message_type::route_auth msg_auth;
	message_type::load_type load2auth;
	buffer.reserve(1024);
	msg_route.set_category(message_type::UPDATE_LOAD);
	msg_auth.set_category(message_type::UPDATE_LOAD);

	while (true) {
		timer.expires_after(interval);
		timer.async_wait([&timer_flag, this](const error_code& ec) {
			if (!ec) {
				timer_flag = false;
				node_info_to_supervisor_.cancel();
			}
		});

		load2auth.set_session_count(business_list_.empty() ? 0 : (self_load_ / business_list_.size()));
		msg_auth.mutable_server_load()->CopyFrom(load2auth);
		auto shared_msg = msg_auth.SerializeAsString();
		for (auto& [key, session] : auth_list_) {
			session->deliver(shared_msg);
		}
		msg_auth.clear_server_load();

		while (timer_flag) {
			buffer = co_await node_info_to_supervisor_.async_receive(token);
			if (!ec) {
				msg_route.add_load_list(std::move(buffer));
			}
			else if (ec && ec != net::error::operation_aborted && ec != expr::error::channel_cancelled) {
				log().error("node_info_distributor: {}, code: {}, name: {}", ec.message(), ec.value(), ec.category().name());
				break;
			}
		}

		shared_msg = msg_route.SerializeAsString();
		for (auto& [key, session] : route_list_) {
			session->deliver(shared_msg);
		}

		co_await timer.async_wait(token);
		if (ec && ec != net::error::operation_aborted && ec != expr::error::channel_cancelled) {
			log().error("node_info_distributor: {}, code: {}, name: {}", ec.message(), ec.value(), ec.category().name());
			break;
		}

		msg_route.clear_load_list();
		timer_flag = true;
	}
}

net::awaitable<void> route_server::push_node_info(const std::string& message, bool to_route) {
	error_code ec;
	auto token = net::redirect_error(net::deferred, ec);
	if (to_route) {
		co_await node_info_to_route_.async_send({}, message, token);
		if (ec && ec != net::error::operation_aborted && ec != expr::error::channel_cancelled) {
			log().error("push_node_info(to route): {}, code: {}, name: {}", ec.message(), ec.value(), ec.category().name());
		}
	}
	co_await node_info_to_supervisor_.async_send({}, message, token);
	if (ec && ec != net::error::operation_aborted && ec != expr::error::channel_cancelled) {
		log().error("push_node_info: {}, code: {}, name: {}", ec.message(), ec.value(), ec.category().name());
	}
}

net::awaitable<void> route_server::load_updater_impl() {
	error_code ec;
	net::steady_timer timer{ ioc_ };
	std::atomic<bool> timer_flag{ true };
	auto token = net::redirect_error(net::use_awaitable, ec);
	auto interval = std::chrono::seconds(config_loader::load_config()["route_update_interval"].get<int>());
	supr::route_supervisor msg_supr;
	std::string buffer;
	buffer.reserve(1024);
	msg_supr.set_category(supr::SERVER_LIST);

	while (true) {
		timer.expires_after(interval);
		timer.async_wait([&timer_flag, this](const error_code& ec) {
			if (!ec) {
				timer_flag = false;
				node_info_to_supervisor_.cancel();
			}
		});
		while (timer_flag) {
			buffer = co_await node_info_to_supervisor_.async_receive(token);
			if (!ec) {
				msg_supr.add_load_list(std::move(buffer));
			}
		}

		auto shared_msg = msg_supr.SerializeAsString();
		for (auto& [key, session] : supervisor_list_) {
			log().debug("load_updater_impl: {}", session->remote_uri());
			session->deliver(shared_msg);
		}

		co_await timer.async_wait(token);
		if (ec && ec != net::error::operation_aborted) {
			log().error("load_updater_impl: {}, code: {}, name: {}", ec.message(), ec.value(), ec.category().name());
			break;
		}

		msg_supr.clear_load_list();
		timer_flag = true;
	}
}

void route_server::task_response_impl(std::string key, std::string message) {
	auto it = auth_list_.find(key);
	if (it != auth_list_.end()) {
		it->second->deliver(message);
	}
	else {
		log().error("auth_server::task_response_impl: client not found");
	}
}

}
}