#pragma once
#ifndef SPARROW_AUTH_SERVER_HPP
#define SPARROW_AUTH_SERVER_HPP

#include "../base/server.hpp"

namespace leo {
namespace auth {
;
class auth_session;
class auth_server
	: public server<auth_server>
	, public std::enable_shared_from_this<auth_server> 
{
	
public:
	auth_server(net::io_context& ioc);
	void start_impl();
	void stop_impl();
	void store_impl();
};

}
}

#endif // !SPARROW_AUTH_SERVER_HPP