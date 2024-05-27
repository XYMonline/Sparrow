#pragma once
#ifndef SPARROW_SERVER_HPP
#define SPARROW_SERVER_HPP

#include "net.hpp"
#include "beast.hpp"
#include "../tools/cancellation_signals.hpp"
#include "../tools/service/cache_service.hpp"
#include "../tools/service/storage_service.hpp"
#include "../tools/config_loader.hpp"
#include "../tools/server_certificate.hpp"

#include <print>

namespace leo {
;

struct connect_config {
	std::string					db_host;
	std::string					db_name;
	std::string					db_user;
	std::string					db_password;
	std::string					cache_user;
	std::string					cache_password;
	std::string					cert_path;
	std::string					key_path;
	std::string					dh_path;
	uint16_t					db_port;
	redis::ConnectionOptions	options;
};

connect_config connect_config_init();

/*
 * derived class must implement the following functions:
 * - void start_impl()
 * - void stop_impl()
 * - void store_impl()
 * - void temp_add_impl(SessionPtr ptr)
 * - void perm_add_impl(std::string key, SessionPtr ptr)
 * - void temp_remove_impl(std::string key)
 * - void perm_remove_impl(std::string key)
 */
template<typename Derived>
class server {
	Derived& derived() {
		return static_cast<Derived&>(*this);
	}

protected:
	net::io_context& ioc_;
	ssl::context ctx_;
	cache_service cache_;
	storage_service storage_;
	cancellation_signals signals_;
	std::string uri_;

	constexpr static const char* table_route_list{ "route_list" };
	constexpr static const char* table_auth_list{ "auth_list" };
	constexpr static const char* table_business_list{ "business_list" };
	constexpr static const char* table_supervisor_list{ "supervisor_list" };

public:
	server(net::io_context& ioc)
		: ioc_{ ioc }
		, ctx_{ ssl::context::tlsv12 }
		, storage_{ ioc, ctx_ } 
	{
	}

	~server() {
		std::println("server::~server");
	}

	void start() {
		error_code ec;
		auto& conf = config_loader::load_config();
		auto [db_host, db_name, db_user, db_password, cache_user, cache_password, cert_path, key_path, dh_path, db_port, options] = connect_config_init();
		
		bool connect_cache = cache_.init(options);

		bool connect_storage = storage_.init(
			db_host,
			db_user,
			db_password,
			db_name,
			db_port
		);

		if (!connect_cache || !connect_storage) {
			std::println("error: server::start: failed to connect to cache or storage");
			return;
		}

		// 先连接到数据库和缓存，再加载证书
		leo::load_server_certificate(
			ctx_,
			ec,
			cert_path,
			key_path,
			dh_path
		);

		derived().start_impl();
	}

	void stop() { 
		derived().stop_impl(); 
	}

	void store() { derived().store_impl(); }

	storage_service& storage() { return storage_; }
	cache_service& cache() { return cache_; }
	cancellation_signals& signals() { return signals_; }

	void set_uri(std::string uri) { uri_ = uri; }
	const std::string& uri() const { return uri_; }

	// add and remove session form temp and permanent container
	template<typename SessionPtr> void temp_add(SessionPtr ptr) { derived().temp_add_impl(std::move(ptr)); }
	template<typename SessionPtr> void perm_add(std::string key, SessionPtr ptr) { derived().perm_add_impl(std::move(key), std::move(ptr)); }
	template<typename SessionPtr> void temp_remove(std::string key) { derived().temp_remove_impl<SessionPtr>(std::move(key)); }
	template<typename SessionPtr> void perm_remove(std::string key) { derived().perm_remove_impl<SessionPtr>(std::move(key)); }
};

}

#endif // !SPARROW_SERVER_HPP
