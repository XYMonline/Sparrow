#include "business_server.hpp"
#include "../base/listener.hpp"
#include "route_session.hpp"
#include "client_session.hpp"

namespace leo {
namespace business {
;
business_server::business_server(net::io_context& ioc)
	: server(ioc) 
{
}

void business_server::start_impl() {
	auto listener = std::make_shared<leo::listener>(ioc_, ctx_);
	listener->run<business_server, client_session>(*this, "business_client_port_range");

	// connect to route server
}

void business_server::stop_impl() {

}

void business_server::store_impl() {

}

}
}
