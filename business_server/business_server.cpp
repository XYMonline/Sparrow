#include "business_server.hpp"
#include "../base/listener.hpp"
#include "route_session.hpp"
#include "client_session.hpp"

#include "../tools/proto/server_message.pb.h"

#include <algorithm>
#include <random>

namespace leo {
namespace business {
;
business_server::business_server(net::io_context& ioc)
	: server{ ioc }
{
}

void business_server::start_impl() {
	bool res{ false };
	auto listener = std::make_shared<leo::listener>(ioc_, ctx_);
	auto port = listener->run<business_server, client_session>(*this, "business_client_port_range");

	if (port) {
		set_uri(std::format("{}:{}", config_loader::load_config()["host"].get<std::string>(), port));
		std::println("listening on port: {}", port);

		res = connect_route();
	}
	else {
		std::println("business_server::start_impl: failed to start listener");
	}

	if (!res) {
		std::println("connect_route failed");
		stop();
		std::exit(1);
	}
}

void business_server::stop_impl() {
	clients_.clear();
	client_temp_.clear();
	route_->stop();
	route_.reset();
}

void business_server::store_impl() {

}

bool business_server::connect_route() {
	boost::system::error_code ec;
	auto route_list = cache_.get_services(table_business_list);
	std::shuffle(route_list.begin(), route_list.end(), std::random_device{});
	for (auto& uri : route_list) {
		auto [host, port] = uri2host_port(uri);
		tcp::resolver resolver{ ioc_ };
		auto endpoints = resolver.resolve(host, port);
		tcp::socket socket{ ioc_ };
		net::connect(socket, endpoints, ec);
		if (ec) {
			std::println("connect_route failed: {} code: {}", ec.message(), ec.value());
			continue;
		}
		route_ = std::make_shared<route_session>(
			beast::ssl_stream<beast::tcp_stream>{
				beast::tcp_stream{ std::move(socket) },
				ctx_
			},
			*this
		);
		route_->set_local_uri(uri_);
		route_->start();
		std::println("connect to route: {}", uri);
		break;
	}

	// 如果没有找到route_server， 直接退出
	return route_ != nullptr;
}

net::awaitable<void> business_server::load_updater_impl() {
	error_code ec;
	auto token = net::redirect_error(net::use_awaitable, ec);
	net::steady_timer timer{ ioc_ };
	auto interval = std::chrono::seconds(config_loader::load_config()["business_update_interval"].get<int>());
	int session_increase{ 0 };
	message_type::load_type load_info;
	message_type::route_business msg;

	msg.set_category(message_type::UPDATE_LOAD);
	load_info.set_address(uri());
	load_info.set_type(message_type::AUTH_SERVER);
	load_info.set_memory_total(memory_total());

	while (true) {
		session_increase = (int)clients_.size() - session_increase;
		load_info.set_session_increase(session_increase);
		load_info.set_cpu_usage(cpu_usage());
		load_info.set_memory_free(memory_free());

		//msg.set_allocated_server_load(&load_info);
		msg.mutable_server_load()->CopyFrom(load_info);
		route_->deliver(msg.SerializeAsString());

		timer.expires_after(interval);
		co_await timer.async_wait(token);
		if (ec && ec != net::error::operation_aborted) {
			std::println("auth_server::load_updater_impl: {}", ec.message());
			break;
		}
	}
}

}
}
