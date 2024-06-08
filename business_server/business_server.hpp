#pragma once
#ifndef SPARROW_BUSINESS_SERVER_HPP
#define SPARROW_BUSINESS_SERVER_HPP

#include "../base/server.hpp"

namespace leo {
namespace business {
;
class client_session;
class route_session;

using client_ptr = std::shared_ptr<client_session>;
using route_ptr = std::shared_ptr<route_session>;
class business_server
	: public server<business_server>
	, public std::enable_shared_from_this<business_server>
{
	route_ptr route_;
	phmap::flat_hash_map<std::string, client_ptr> clients_, client_temp_;
public:
	business_server(net::io_context& ioc);
	void start_impl();
	void stop_impl();
	void store_impl();

	bool connect_route();

	template<typename SessionPtr> void temp_add_impl(SessionPtr ptr) {}
	template<typename SessionPtr> void perm_add_impl(std::string key, SessionPtr ptr) {}
	template<typename SessionPtr> void temp_remove_impl(std::string key) {}
	template<typename SessionPtr> void perm_remove_impl(std::string key) {}

	net::awaitable<void> load_updater_impl();
};

}
}

#endif // !SPARROW_BUSINESS_SERVER_HPP