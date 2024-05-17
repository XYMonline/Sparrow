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

public:
	server(net::io_context& ioc)
		: ioc_{ ioc }
		, ctx_{ ssl::context::tlsv12 }
		, storage_{ ioc, ctx_ } {
	}

	void start() {
		error_code ec;
		auto& conf = config_loader::load_config();
		std::string db_host, db_name, db_user, db_password, cache_user, cache_password;
		uint16_t db_port{ 3306 };
		redis::ConnectionOptions options;

		//////////////////////////////////////
		db_user = "root";
		db_password = "1234";
		db_host = "localhost";
		db_name = "sparrow";
		/////////////////////////////////////

		options.host = conf["cache_host"].get<std::string>();
		if (conf.contains("cache_port"))
			options.port = conf["cache_port"].get<int>();
		if (conf.contains("cache_user"))
			options.user = conf["cache_user"].get<std::string>();
		if (conf.contains("cache_password"))
			options.password = conf["cache_password"].get<std::string>();

		if (conf.contains("db_user"))
			db_user = conf["db_user"].get<std::string>();
		if (conf.contains("db_port"))
			db_port = conf["db_port"].get<uint16_t>();
		if (conf.contains("db_name"))
			db_name = conf["db_name"].get<std::string>();


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
			conf["cert_path"].get<std::string>(),
			conf["private_key_path"].get<std::string>(),
			conf["dh_path"].get<std::string>()
		);

		derived().start_impl();
	}
	void stop() { derived().stop_impl(); }
	void store() { derived().store_impl(); }

	storage_service& storage() { return storage_; }
	cache_service& cache() { return cache_; }
	cancellation_signals& signals() { return signals_; }

};
}

#endif // !SPARROW_SERVER_HPP
