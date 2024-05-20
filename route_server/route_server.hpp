#pragma once
#ifndef SPARROW_ROUTE_SERVER_HPP
#define SPARROW_ROUTE_SERVER_HPP

#include "../base/server.hpp"

namespace leo {
namespace route {
;
class auth_session;
class route_session;
class business_session;
class controler_session;

class route_server
	: public server<route_server>
	, public std::enable_shared_from_this<route_server>
{

public:
	route_server(net::io_context& ioc);
	void start_impl();
	void stop_impl();
	void store_impl();
};

}
}

#endif // !SPARROW_ROUTE_SERVER_HPP