#pragma once
#ifndef SPARROW_BUSINESS_SERVER_HPP
#define SPARROW_BUSINESS_SERVER_HPP

#include "../base/server.hpp"
#include "../tools/wrap_container.hpp"

namespace leo {
namespace business {
;
class business_session;
class route_session;

using business_ptr = std::shared_ptr<business_session>;
using route_ptr = std::shared_ptr<route_session>;
class business_server
	: public server<business_server>
	, public std::enable_shared_from_this<business_server>
{
	route_ptr route_;
	wrap_map<std::string, business_ptr> business_list_, business_temp_;
public:
	business_server(net::io_context& ioc);
	void start_impl();
	void stop_impl();
	void store_impl();

	void connect_route();
};

}
}

#endif // !SPARROW_BUSINESS_SERVER_HPP