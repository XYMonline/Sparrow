#pragma once
#ifndef SPARROW_ROUTE_SERVER_HPP
#define SPARROW_ROUTE_SERVER_HPP

#include "../base/server.hpp"
#include "../tools/wrap_container.hpp"
#include "../tools/load_blancer.hpp"

namespace leo {
namespace route {
;
class auth_session;
class route_session;
class business_session;
class controller_session;

using auth_ptr = std::shared_ptr<auth_session>;
using route_ptr = std::shared_ptr<route_session>;
using business_ptr = std::shared_ptr<business_session>;
using controller_ptr = std::shared_ptr<controller_session>;

class route_server
	: public server<route_server>
	, public std::enable_shared_from_this<route_server>
{
	wrap_map<std::string, auth_ptr>				auth_list_, auth_temp_;
	wrap_map<std::string, route_ptr>			route_in_, route_out_, route_temp_; // in -> 监听到的连接, out -> 连接到的服务器, temp -> 临时存储
	wrap_map<std::string, business_ptr>			business_list_, business_temp_;
	wrap_map<std::string, controller_ptr>		controller_list_, controller_temp_;
	load_balancer<business_session, least_connections>	business_lb_;

	uint16_t auth_port_{ 0 };
	uint16_t route_port_{ 0 };
	uint16_t business_port_{ 0 };
	uint16_t controller_port_{ 0 };

private:
	void connect_route();

public:
	route_server(net::io_context& ioc);
	void start_impl();
	void stop_impl();
	void store_impl();

	template<typename SessionPtr> void temp_add_impl(SessionPtr ptr);
	template<typename SessionPtr> void perm_add_impl(std::string key, SessionPtr ptr);
	template<typename SessionPtr> void temp_remove_impl(std::string key);
	template<typename SessionPtr> void perm_remove_impl(std::string key);
};

template<typename SessionPtr>
inline void route_server::temp_add_impl(SessionPtr ptr) {
	bool res{ false };
	if constexpr (std::is_same_v<SessionPtr, auth_ptr>) {
		res = auth_temp_.emplace(ptr->uuid(), ptr).second; // if the key is already exist, it will return pair<iterator, false>
	}
	else if constexpr (std::is_same_v<SessionPtr, route_ptr>) {
		res = route_temp_.emplace(ptr->uuid(), ptr).second;
	}
	else if constexpr (std::is_same_v<SessionPtr, business_ptr>) {
		res = business_temp_.emplace(ptr->uuid(), ptr).second;
	}
	else if constexpr (std::is_same_v<SessionPtr, controller_ptr>) {
		res = controller_temp_.emplace(ptr->uuid(), ptr).second;
	}
	if (res) {
		std::println("temp_session: {} join", ptr->uuid());
	}
	else {
		std::println("temp_session: {} already exist", ptr->uuid());
	}
}

template<typename SessionPtr>
inline void route_server::perm_add_impl(std::string key, SessionPtr ptr) { // key = uri or other consistent key
	bool res{ false };
	if constexpr (std::is_same_v<SessionPtr, auth_ptr>) {
		if (auth_list_.emplace(key, ptr).second) {
			//auth_temp_.erase(ptr->uuid());
			res = true;
		}
	}
	else if constexpr (std::is_same_v<SessionPtr, route_ptr>) {
		if (route_in_.emplace(key, ptr).second) {
			//route_temp_.erase(ptr->uuid());
			res = true;
		}
	}
	else if constexpr (std::is_same_v<SessionPtr, business_ptr>) {
		if (business_list_.emplace(key, ptr).second) {
			business_lb_.add_server(key, ptr);
			//business_temp_.erase(ptr->uuid());
			res = true;
		}
	}
	else if constexpr (std::is_same_v<SessionPtr, controller_ptr>) {
		if (controller_list_.emplace(key, ptr).second) {
			//controller_temp_.erase(ptr->uuid());
			res = true;
		}
	}
	if (res) {
		std::println("perm_session: {} join, uuid: {}", key, ptr->uuid());
		this->temp_remove<SessionPtr>(ptr->uuid());
	}
	else {
		std::println("perm_session: {} already exist", key);
	}
}

template<typename SessionPtr>
inline void route_server::temp_remove_impl(std::string key) {
	bool res{ false };
	if constexpr (std::is_same_v<SessionPtr, auth_ptr>) {
		res = auth_temp_.erase(key);
	}
	else if constexpr (std::is_same_v<SessionPtr, route_ptr>) {
		res = route_temp_.erase(key);
	}
	else if constexpr (std::is_same_v<SessionPtr, business_ptr>) {
		res = business_temp_.erase(key);
		business_lb_.remove_server(key);
	}
	else if constexpr (std::is_same_v<SessionPtr, controller_ptr>) {
		res = controller_temp_.erase(key);
	}
	if (res) {
		std::println("temp_session: {} leave", key);
	}
	else {
		std::println("temp_session: {} not exist", key);
	}
}

template<typename SessionPtr>
inline void route_server::perm_remove_impl(std::string key) {
	bool res{ false };
	if constexpr (std::is_same_v<SessionPtr, auth_ptr>) {
		res = auth_list_.erase(key);
	}
	else if constexpr (std::is_same_v<SessionPtr, route_ptr>) {
		res = route_in_.erase(key) || route_out_.erase(key); // erase both in and out
	}
	else if constexpr (std::is_same_v<SessionPtr, business_ptr>) {
		business_lb_.remove_server(key);
		res = business_list_.erase(key);
	}
	else if constexpr (std::is_same_v<SessionPtr, controller_ptr>) {
		res = controller_list_.erase(key);
	}
	if (res) {
		std::println("perm_session: {} leave", key);
	}
}

}
}

#endif // !SPARROW_ROUTE_SERVER_HPP