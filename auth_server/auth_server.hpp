#pragma once
#ifndef SPARROW_AUTH_SERVER_HPP
#define SPARROW_AUTH_SERVER_HPP

#include "../base/server.hpp"
#include "../tools/load_balancer.hpp"

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
	phmap::flat_hash_map<std::string, client_ptr>	clients_, client_temp_;
	phmap::flat_hash_map<std::string, route_ptr>	routes_;
	load_balancer<route_session, least_connections> route_lb_;

public:
	auth_server(net::io_context& ioc);

private:
	friend server<auth_server>;

	void start_impl();
	void stop_impl();
	void store_impl();

	template<typename Func>
	void task_request_impl(Func&& task) {
		route_lb_.commit(std::forward<Func>(task));
	}

	void task_response_impl(std::string key, std::string message);

	template<typename SessionPtr> void temp_add_impl(SessionPtr ptr);
	template<typename SessionPtr> void perm_add_impl(std::string key, SessionPtr ptr);
	template<typename SessionPtr> void temp_remove_impl(std::string key);
	template<typename SessionPtr> void perm_remove_impl(std::string key);

	net::awaitable<void> load_updater_impl();

public:

	// connect to route server
	bool connect_route();
	// check route list , reconnect if empty
	void check_routes();

	void make_route_session(const std::string& uri);
};

template<typename SessionPtr>
inline void auth_server::temp_add_impl(SessionPtr ptr) {
	bool res{ false };
	if constexpr (std::is_same_v<SessionPtr, client_ptr>) {
		res = client_temp_.try_emplace(ptr->uuid(), ptr).second;
	}
	if (res) {
		log().debug("temp_session: {} join", ptr->uuid());
	}
	else {
		log().debug("temp_session: {} already exist", ptr->uuid());
	}
}

template<typename SessionPtr>
inline void auth_server::perm_add_impl(std::string key, SessionPtr ptr) {
	bool res{ false };
	if constexpr (std::is_same_v<SessionPtr, client_ptr>) {
		if (clients_.try_emplace(key, ptr).second) {
			client_temp_.erase(ptr->uuid());
			res = true;
		}
	}
	else if constexpr (std::is_same_v<SessionPtr, route_ptr>) {
		if (routes_.try_emplace(key, ptr).second) {
			route_lb_.add_server(key, ptr);
			// dont print route join message
			return;
		}
	}
	if (res) {
		log().debug("perm_session: {} join", key);
	}
	else {
		log().debug("perm_session: {} already exist", key);
	}
}

template<typename SessionPtr>
inline void auth_server::temp_remove_impl(std::string key) {
	bool res{ false };
	if constexpr (std::is_same_v<SessionPtr, client_ptr>) {
		res = client_temp_.erase(key);
	}
	if (res) {
		log().debug("temp_session: {} leave", key);
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
		log().debug("perm_session: {} leave", key);
	}
}

}
}

#endif // !SPARROW_AUTH_SERVER_HPP