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
	auto listener = std::make_shared<leo::listener>(ioc_, ctx_);
	auto port = listener->run<auth_server, client_session>(*this, "auth_client_port_range");
	if (port) {
		set_uri(std::format("{}:{}", config_loader::load_config()["host"].get<std::string>(), port));
		std::println("listening on port: {}", port);
	}

	connect_route();
}

void auth_server::stop_impl() {
	clients_.clear();
	client_temp_.clear();
	routes_.clear();
	route_lb_.stop();
}

void auth_server::store_impl() {

}

void auth_server::connect_route() {
	boost::system::error_code ec;
	auto host = config_loader::load_config()["host"].get<std::string>();
	auto route_list = cache_.get_services(table_auth_list);
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
		auto route = std::make_shared<route_session>(
			beast::ssl_stream<beast::tcp_stream>{
				beast::tcp_stream{ std::move(socket) },
				ctx_
			},
			*this
		);
		route->set_uri(uri_);
		route->start();
		perm_add(uri, route);
		std::println("connect to route: {}", uri);
	}
}

}
}