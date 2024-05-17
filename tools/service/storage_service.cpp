#include "storage_service.hpp"

#include <print>

namespace leo {
;
storage_service::storage_service(net::io_context& ioc, ssl::context& ctx, size_t connection_num)
	: connection_pool_{ ioc, connection_num }
	, connection_num_{ connection_num }
	, ctx_{ ctx } {
}
bool storage_service::init(std::string_view host, std::string_view user, std::string_view password, std::string_view db, std::uint16_t port) {
	size_t count{ 0 };
	try {
		tcp::resolver resolver{ connection_pool_.get_executor() };
		auto result = resolver.resolve(host, std::to_string(port));
		auto endpoint = *result.begin();
		auto params = mysql::handshake_params{ user, password, db };

		for (; count < connection_num_; ) {
			auto conn = std::make_shared<connection_type>(connection_pool_.get_executor(), ctx_);
			conn->connect(endpoint, params);
			bool res = connection_pool_.try_send(error_code{}, conn);
			if (res) {
				++count;
			}
		}
	}
	catch (const std::exception& e) {
		std::println(" error: storage_service::init: {}", e.what());
		return false;
	}
	return count == connection_num_;
}
}
