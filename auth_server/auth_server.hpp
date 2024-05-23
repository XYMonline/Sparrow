#pragma once
#ifndef SPARROW_AUTH_SERVER_HPP
#define SPARROW_AUTH_SERVER_HPP

#include "../base/server.hpp"
#include "../tools/load_blancer.hpp"
#include "../tools/wrap_container.hpp"

namespace leo {
namespace auth {
;
class client_session;
class route_session;

using client_ptr = std::shared_ptr<client_session>;
using route_ptr = std::shared_ptr<route_session>;

class auth_server
	: public server<auth_server>
	, public std::enable_shared_from_this<auth_server> 
{
	wrap_map<std::string, client_ptr>	clients_, client_temp_;
	wrap_map<std::string, route_ptr>	routes_, route_temp_;
	load_balancer<route_ptr, least_connections> route_lb_;

public:
	auth_server(net::io_context& ioc);
	void start_impl();
	void stop_impl();
	void store_impl();

	template<typename SessionPtr> void temp_add_impl(SessionPtr ptr);
	template<typename SessionPtr> void perm_add_impl(std::string key, SessionPtr ptr);
	template<typename SessionPtr> void temp_remove_impl(std::string key);
	template<typename SessionPtr> void perm_remove_impl(std::string key);

private:
	void connect_route();
};

template<typename SessionPtr>
inline void auth_server::temp_add_impl(SessionPtr ptr) {
	bool res{ false };
	if constexpr (std::is_same_v<SessionPtr, client_ptr>) {
		res = client_temp_.emplace(ptr->uuid(), ptr).second;
	}
	else if constexpr (std::is_same_v<SessionPtr, route_ptr>) {
		res = route_temp_.emplace(ptr->uuid(), ptr).second;
	}
	if (res) {
		std::println("temp_session: {} join", ptr->uuid());
	}
	else {
		std::println("temp_session: {} already exist", ptr->uuid());
	}
}

template<typename SessionPtr>
inline void auth_server::perm_add_impl(std::string key, SessionPtr ptr) {
	bool res{ false };
	if constexpr (std::is_same_v<SessionPtr, client_ptr>) {
		if (clients_.emplace(key, ptr).second) {
			client_temp_.erase(ptr->uuid());
			res = true;
		}
	}
	else if constexpr (std::is_same_v<SessionPtr, route_ptr>) {
		if (routes_.emplace(key, ptr).second) {
			route_lb_.add_server(key, ptr);
			route_temp_.erase(ptr->uuid());
			res = true;
		}
	}
	if (res) {
		std::println("perm_session: {} join", key);
	}
	else {
		std::println("perm_session: {} already exist", key);
	}
}

template<typename SessionPtr>
inline void auth_server::temp_remove_impl(std::string key) {
	bool res{ false };
	if constexpr (std::is_same_v<SessionPtr, client_ptr>) {
		res = client_temp_.erase(key);
	}
	else if constexpr (std::is_same_v<SessionPtr, route_ptr>) {
		res = route_temp_.erase(key);
	}
	if (res) {
		std::println("temp_session: {} leave", key);
	}
	else {
		std::println("temp_session: {} not found", key);
	}
}

template<typename SessionPtr>
inline void auth_server::perm_remove_impl(std::string key) {
	bool res{ false };
	if constexpr (std::is_same_v<SessionPtr, client_ptr>) {
		res = clients_.erase(key);
	}
	else if constexpr (std::is_same_v<SessionPtr, route_ptr>) {
		route_lb_.remove_server(key);
		res = routes_.erase(key);
	}
	if (res) {
		std::println("perm_session: {} leave", key);
	}
	else {
		std::println("perm_session: {} not found", key);
	}
}

}
}

#endif // !SPARROW_AUTH_SERVER_HPP