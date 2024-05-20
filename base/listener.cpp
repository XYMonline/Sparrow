#include "listener.hpp"

namespace leo {
;
void listener::fail(boost::system::error_code ec, char const* what) {
	if (ec == net::error::operation_aborted) {
		return;
	}
	std::println("{}: {} code: {}", what, ec.message(), ec.value());
}

bool listener::acceptor_init(std::string_view port_range_key) {
	auto conf{ config_loader::load_config() };
	auto host{ conf["host"].get<std::string>() };
	beast::error_code ec;
	auto [port_start, port_stop] = conf[port_range_key].get<std::tuple<uint16_t, uint16_t>>();
	bool init{ false };

	for (; port_start <= port_stop; ++port_start) {
		auto addr{ net::ip::make_address(host) };
		tcp::endpoint endpoint{ addr, port_start };

		acceptor_ = std::make_unique<tcp::acceptor>(ioc_);

		acceptor_->open(endpoint.protocol(), ec);
		if (ec) {
			fail(ec, "open");
			continue;
		}

		acceptor_->set_option(net::socket_base::reuse_address(true), ec);
		if (ec) {
			fail(ec, "set_option");
			continue;
		}

		acceptor_->bind(endpoint, ec);
		if (ec) {
			fail(ec, "bind");
			continue;
		}

		acceptor_->listen(net::socket_base::max_listen_connections, ec);
		if (ec) {
			fail(ec, "listen");
			continue;
		}
		else {
			init = true;
			std::println("server start on {}:{}", host, port_start);
			break;
		}
	}

	return init;
}

}
