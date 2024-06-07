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

	net::co_spawn(
		ioc,
		load_updater_impl(),
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
		std::println("listening autn_server on port: {}", auth_port_);
	}
	if (business_port_) {
		cache_.signup_service(table_business_list, std::format("{}:{}", host, business_port_));
		std::println("listening business_server on port: {}", business_port_);
	}
	if (route_port_) {
		cache_.signup_service(table_route_list, std::format("{}:{}", host, route_port_));
		std::println("listening route_server on port: {}", route_port_);
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
			std::println("connect_route failed: {} code: {}", ec.message(), ec.value());
			continue;
		}

		route->start();
		std::println("connect to route: {}", uri);

		message_type::route_route msg;
		msg.set_category(message_type::SERVER_INFO);
		msg.set_uri(uri_);
		route->deliver(msg.SerializeAsString());

		find_route = true;
	}

	if (!find_route) {
		std::println("no another route server found");
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
			std::println("route_info_distributor: {}, code: {}, name: {}", ec.message(), ec.value(), ec.category().name());
			break;
		}

		auto msg = std::make_shared<std::string>(std::move(message));
		auth_list_.for_each([msg](auto& session) {
			std::println("auth_list_ session: {}", session->remote_uri());
			session->deliver(*msg);
		});
	}
}

net::awaitable<void> route_server::push_route_info(const std::string& message) {
	error_code ec;
	auto token = net::redirect_error(net::deferred, ec);
	co_await route_info_channel_.async_send({}, message, token);
	if (ec && ec != net::error::operation_aborted) {
		std::println("push_route_info: {}, code: {}, name: {}", ec.message(), ec.value(), ec.category().name());
	}
}

net::awaitable<void> route_server::node_info_distributor() {
	error_code ec;
	net::steady_timer timer{ ioc_ };
	std::atomic<bool> timer_flag{ true };
	auto token = net::redirect_error(net::deferred, ec);
	auto interval = std::chrono::seconds(config_loader::load_config()["route_update_interval"].get<int>());
	std::string buffer;
	message_type::route_route msg_route;
	buffer.reserve(1024);
	msg_route.set_category(message_type::UPDATE_LOAD);

	while (true) {
		timer.expires_after(interval);
		timer.async_wait([&timer_flag, this](const error_code& ec) {
			if (!ec) {
				timer_flag = false;
				node_info_to_route_.cancel();
			}
		});
		while (timer_flag) {
			buffer = co_await node_info_to_supervisor_.async_receive(token);
			if (!ec) {
				msg_route.add_load_list(std::move(buffer));
			}
			else if (ec && ec != net::error::operation_aborted && ec != expr::error::channel_cancelled) {
				std::println("node_info_distributor: {}, code: {}, name: {}", ec.message(), ec.value(), ec.category().name());
				break;
			}
		}

		auto shared_msg = std::make_shared<std::string>(msg_route.SerializeAsString());
		route_list_.for_each([shared_msg](auto& session) {
			std::println("roiute_list_ session: {}", session->remote_uri());
			session->deliver(*shared_msg);
			});

		co_await timer.async_wait(token);
		if (ec && ec != net::error::operation_aborted && ec != expr::error::channel_cancelled) {
			std::println("route_server::node_info_distributor: {}", ec.message());
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
			std::println("push_node_info: {}, code: {}, name: {}", ec.message(), ec.value(), ec.category().name());
		}
	}
	co_await node_info_to_supervisor_.async_send({}, message, token);
	if (ec && ec != net::error::operation_aborted && ec != expr::error::channel_cancelled) {
		std::println("push_node_info: {}, code: {}, name: {}", ec.message(), ec.value(), ec.category().name());
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

		auto shared_msg = std::make_shared<std::string>(msg_supr.SerializeAsString());
		supervisor_list_.for_each([shared_msg](auto& session) {
			std::println("supervisor_list_ session: {}", session->remote_uri());
			session->deliver(*shared_msg);
			});

		co_await timer.async_wait(token);
		if (ec && ec != net::error::operation_aborted) {
			std::println("route_server::load_updater_impl: {}", ec.message());
			break;
		}

		msg_supr.clear_load_list();
		timer_flag = true;
	}
}

}
}