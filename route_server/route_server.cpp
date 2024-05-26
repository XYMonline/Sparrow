#include "route_server.hpp"
#include "../base/listener.hpp"
#include "auth_session.hpp"
#include "route_session.hpp"
#include "business_session.hpp"
#include "controller_session.hpp"

#include "../tools/proto/server_message.pb.h"

#include <format>

namespace leo {
namespace route {
;
route_server::route_server(net::io_context& ioc)
	: server{ ioc }
	, business_lb_{ ioc }
{
}

void route_server::start_impl() {
	auto listener		= std::make_shared<leo::listener>(ioc_, ctx_);
	auth_port_			= listener->run<route_server, auth_session>(*this, "route_auth_port_range");
	route_port_			= listener->run<route_server, route_session>(*this, "route_route_port_range");
	business_port_		= listener->run<route_server, business_session>(*this, "route_business_port_range");
	controller_port_	= listener->run<route_server, controller_session>(*this, "route_controller_port_range");

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
	if (controller_port_) {
		cache_.remove_service(table_controller_list, std::format("{}:{}", host, controller_port_));
	}

	// clear all the sessions
	auth_list_.clear(); 
	auth_temp_.clear();
	route_list_.clear();
	route_temp_.clear();
	business_list_.clear(); 
	business_temp_.clear();
	controller_list_.clear();
	controller_temp_.clear();
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

	route->set_role(ssl::stream_base::client);
	route->set_uri(uri_);

	// 将新建的route_session加入发起连接的列表
	perm_add(uri, route);
	return route;
}

}
}