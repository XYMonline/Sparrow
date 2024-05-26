#include "auth_server.hpp"
#include "client_session.hpp"
#include "route_session.hpp"

#include "../base/listener.hpp"
#include "../tools/service/cache_service.hpp"
#include "../tools/proto/server_message.pb.h"

namespace leo {
namespace auth {
;

auth_server::auth_server(net::io_context& ioc) 
	: server{ ioc }
	, route_lb_{ ioc }
{
}

void auth_server::start_impl() {
	bool res{ false };
	auto listener = std::make_shared<leo::listener>(ioc_, ctx_);
	auto port = listener->run<auth_server, client_session>(*this, "auth_client_port_range");
	if (port) {
		set_uri(std::format("{}:{}", config_loader::load_config()["host"].get<std::string>(), port));
		std::println("listening on port: {}", port);

		res = connect_route();
	}
	else {
		std::println("auth_server::start_impl: failed to start listener");
	}

	// 连接不到route_server就停止
	if (!res) {
		std::println("connect_route failed");
		stop();
		std::exit(1);
	}
}

void auth_server::stop_impl() {
	clients_.clear();
	client_temp_.clear();
	routes_.clear();
	route_lb_.stop();
}

void auth_server::store_impl() {

}

bool auth_server::connect_route() {
	boost::system::error_code ec;
	auto host = config_loader::load_config()["host"].get<std::string>();
	auto route_list = cache_.get_services(table_auth_list);
	for (auto& uri : route_list) {
		if (routes_.contains(uri)) {
			continue;
		}
		auto [host, port] = uri2host_port(uri);
		tcp::resolver resolver{ ioc_ };
		auto endpoints = resolver.resolve(host, port);
		tcp::socket socket{ ioc_ };
		net::connect(socket, endpoints, ec);
		if (ec) {
			std::println("connect_route failed: {} code: {}", ec.message(), ec.value());
			continue;
		}
		auto route = std::make_shared<route_session>(
			beast::ssl_stream<beast::tcp_stream>{
				beast::tcp_stream{ std::move(socket) },
				ctx_
			},
			*this
		);
		route->set_route_uri(uri);
		route->set_uri(uri_);
		route->start();
		perm_add(uri, route);
		std::println("connect to route: {}", uri);
	}

	return !routes_.empty();
}

void auth_server::check_routes() {
	if (routes_.empty()) {
		bool res = connect_route();
		if (!res) {
			std::println("connect_route failed");
			stop();
			std::exit(1);
		}
	}
}

}
}