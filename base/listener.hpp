#pragma once
#ifndef SPARROW_LISTENER_HPP
#define SPARROW_LISTENER_HPP

#include "net.hpp"
#include "beast.hpp"
#include "../tools/cancellation_signals.hpp"
#include "../tools/config_loader.hpp"

#include <print>

namespace leo {
;
template<typename Server, typename Session>
void make_session(tcp::socket&& socket, ssl::context& ctx, Server& server) {

	auto session = std::make_shared<Session>(
		beast::ssl_stream<beast::tcp_stream>{
		beast::tcp_stream{ std::move(socket) },
			ctx
	},
		server
	);
	session->start();
}

class listener
	: public std::enable_shared_from_this<listener>
{
	net::io_context& ioc_;
	ssl::context& ctx_;
	std::unique_ptr<tcp::acceptor> acceptor_;

	void fail(boost::system::error_code ec, char const* what) {
		if (ec == net::error::operation_aborted) {
			return;
		}
		std::println("{}: {} code: {}", what, ec.message(), ec.value());
	}

public:
	listener(
		net::io_context& ioc,
		ssl::context& ctx)
		: ioc_(ioc)
		, ctx_(ctx) {

	}

	~listener() {
		std::println("listener destructed");
	}

	bool acceptor_init(std::string_view port_range_key);

	template<typename Server, typename Session>
	net::awaitable<void> start_accept(auto self, Server& server) {
		boost::system::error_code ec;
		auto token = net::redirect_error(net::use_awaitable, ec);
		using stream_type = typename beast::tcp_stream::rebind_executor<executor_with_default>::other;
		while ((co_await net::this_coro::cancellation_state).cancelled() == net::cancellation_type::none) {
			tcp::socket socket = co_await acceptor_->async_accept(token);
			if (!ec) {
				make_session<Server, Session>(std::move(socket), ctx_, server);
			}
			else {
				fail(ec, "accept");
			}
		}
	}

	template<typename Server, typename Session>
	void run(Server& server, std::string_view port_range_key) {
		bool init{ acceptor_init(port_range_key) };

		if (!init) {
			std::println("Listener not initialized");
			return;
		}

		try {
			auto self = shared_from_this();
			net::co_spawn(
				ioc_,
				start_accept<Server, Session>(self, server),
				net::bind_cancellation_slot(server.signals().slot(), net::detached));
		}
		catch (std::exception const& e) {
			std::println("Error1: {}", e.what());
		}
	}
};

} // namespace leo

#endif // !SPARROW_LISTENER_HPP
