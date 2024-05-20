#include "auth_server.hpp"
#include "client_session.hpp"

#include "../base/listener.hpp"

namespace leo {
namespace auth {
;

auth_server::auth_server(net::io_context& ioc) 
	: server{ ioc }
{
}

void auth_server::start_impl() {
	auto listener = std::make_shared<leo::listener>(ioc_, ctx_);
	listener->run<auth_server, client_session>(*this, "login_client_port_range");

	// connect to route server
}

void auth_server::stop_impl() {

}

void auth_server::store_impl() {

}

}
}