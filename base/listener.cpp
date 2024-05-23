#include "listener.hpp"

namespace leo {
;
void listener::fail(boost::system::error_code ec, char const* what) {
	if (ec == net::error::operation_aborted) {
		return;
	}
	std::println("{}: {} code: {}", what, ec.message(), ec.value());
}

uint16_t listener::acceptor_init(std::string_view port_range_key) {
	auto conf{ config_loader::load_config() };
	auto host{ conf["host"].get<std::string>() };
	uint16_t port_start{ 0 }, port_stop{ 0 }, port{ 0 };
	beast::error_code ec;
	try {
		std::tie(port_start, port_stop) = conf[port_range_key].get<std::array<uint16_t, 2>>();
	}
	catch (std::exception const& e) {
		std::println("config error: {}", e.what());
		return 0;
	}

	for (; port_start <= port_stop; ++port_start) {
		auto addr{ net::ip::make_address(host) };
		tcp::endpoint endpoint{ addr, port_start };

		auto acceptor = std::make_unique<tcp::acceptor>(ioc_);

		acceptor->open(endpoint.protocol(), ec);
		if (ec) {
			fail(ec, "open");
			continue;
		}

		acceptor->set_option(net::socket_base::reuse_address(true), ec);
		if (ec) {
			fail(ec, "set_option");
			continue;
		}

		acceptor->bind(endpoint, ec);
		if (ec) {
			fail(ec, "bind");
			continue;
		}

		acceptor->listen(net::socket_base::max_listen_connections, ec);
		if (ec) {
			fail(ec, "listen");
			continue;
		}
		else {
			acceptors_.push_back(std::move(acceptor));
			port = port_start;
			break;
		}
	}

	return port;
}

}
