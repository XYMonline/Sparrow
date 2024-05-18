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

public:
	server(net::io_context& ioc)
		: ioc_{ ioc }
		, ctx_{ ssl::context::tlsv12 }
		, storage_{ ioc, ctx_ } {
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
	void stop() { derived().stop_impl(); }
	void store() { derived().store_impl(); }

	storage_service& storage() { return storage_; }
	cache_service& cache() { return cache_; }
	cancellation_signals& signals() { return signals_; }

	void set_uri(std::string uri) { uri_ = uri; }
	const std::string& uri() const { return uri_; }
};

}

#endif // !SPARROW_SERVER_HPP
