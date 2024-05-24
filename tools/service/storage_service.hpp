#pragma once
#ifndef SPARROW_STORAGE_SERVICE_HPP
#define SPARROW_STORAGE_SERVICE_HPP

#include "../../base/net.hpp"

#include <boost/mysql.hpp>

#include <string_view>

namespace mysql = boost::mysql;

namespace leo {
;
class storage_service {
	using connection_type = boost::mysql::tcp_ssl_connection;
	using connection_ptr = std::shared_ptr<connection_type>;

	expr::concurrent_channel<void(error_code, connection_ptr)> connection_pool_;
	ssl::context& ctx_;
	const size_t connection_num_;

public:
	storage_service(net::io_context& ioc, ssl::context& ctx, size_t connection_num = 8ull);
	~storage_service() {
		connection_pool_.cancel();
	}

	bool init(std::string_view host, std::string_view user, std::string_view password, std::string_view db, std::uint16_t port = 3306);
	void stop() {
		connection_pool_.cancel();
	}

};
}

#endif // !SPARROW_STORAGE_SERVICE_HPP
