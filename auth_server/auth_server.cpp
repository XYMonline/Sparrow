#include "auth_server.hpp"
#include "auth_session.hpp"

#include "../base/listener.hpp"

namespace leo {
namespace auth {
;

auth_server::auth_server(net::io_context& ioc) 
	: server{ ioc }
{
}

void auth_server::start_impl() {
	boost::system::error_code ec;

	auto listener = std::make_shared<leo::listener>(ioc_, ctx_);
	listener->run<auth_server, auth_session>(*this, "login_client_port_range");
}

void auth_server::stop_impl() {

}

void auth_server::store_impl() {

}

}
}