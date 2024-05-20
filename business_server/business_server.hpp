#pragma once
#ifndef SPARROW_BUSINESS_SERVER_HPP
#define SPARROW_BUSINESS_SERVER_HPP

#include "../base/server.hpp"

namespace leo {
namespace business {
;
class business_session;
class business_server
	: public server<business_server>
	, public std::enable_shared_from_this<business_server>
{

public:
	business_server(net::io_context& ioc);
	void start_impl();
	void stop_impl();
	void store_impl();
};

}
}

#endif // !SPARROW_BUSINESS_SERVER_HPP