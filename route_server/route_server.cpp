#include "route_server.hpp"
#include "../base/listener.hpp"
#include "auth_session.hpp"
#include "route_session.hpp"
#include "business_session.hpp"
#include "controler_session.hpp"

namespace leo {
namespace route {
;
route_server::route_server(net::io_context& ioc)
	: server(ioc) 
{
}

void route_server::start_impl() {
	auto listener = std::make_shared<leo::listener>(ioc_, ctx_);
	listener->run<route_server, auth_session>(*this, "route_login_port_range");
	listener->run<route_server, route_session>(*this, "route_route_port_range");
	listener->run<route_server, business_session>(*this, "route_business_port_range");
	listener->run<route_server, controler_session>(*this, "route_controler_port_range");
}

void route_server::stop_impl() {
}

void route_server::store_impl() {
}

}
}