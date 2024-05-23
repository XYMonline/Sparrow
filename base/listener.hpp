#pragma once
#ifndef SPARROW_LISTENER_HPP
#define SPARROW_LISTENER_HPP

#include "net.hpp"
#include "beast.hpp"
#include "../tools/cancellation_signals.hpp"
#include "../tools/config_loader.hpp"

#include <print>
#include <vector>

namespace leo {
;
class listener
	: public std::enable_shared_from_this<listener>
{
	net::io_context& ioc_;
	ssl::context& ctx_;
	std::vector<std::unique_ptr<tcp::acceptor>> acceptors_;

	void fail(boost::system::error_code ec, char const* what);

public:
	listener(
		net::io_context& ioc,
		ssl::context& ctx)
		: ioc_(ioc)
		, ctx_(ctx) 
	{
		acceptors_.reserve(4);
	}

	~listener() {
		std::println("listener destructed");
	}

	uint16_t acceptor_init(std::string_view port_range_key);

	template<typename Server, typename Session>
	uint16_t run(Server& server, std::string_view port_range_key) { // return listening port
		uint16_t port{ acceptor_init(port_range_key) };

		if (!port) {
			std::println("Listener not initialized");
			return port;
		}

		auto self = shared_from_this();
		net::co_spawn(
			ioc_,
			this->start_accept<Server, Session>(self, server, acceptors_.back(), port),
			net::bind_cancellation_slot(server.signals().slot(), net::detached));

		return port;
	}

	template<typename Server, typename Session>
	net::awaitable<void> start_accept(auto self, Server& server, std::unique_ptr<tcp::acceptor>& acceptor, uint16_t listen_port) {
		boost::system::error_code ec;
		auto token = net::redirect_error(net::use_awaitable, ec);
		while ((co_await net::this_coro::cancellation_state).cancelled() == net::cancellation_type::none) {
			tcp::socket socket = co_await acceptor->async_accept(token);
			if (!ec) {
				//std::println("{}", typeid(Session).name());
				auto session = std::make_shared<Session>(
					beast::ssl_stream<beast::tcp_stream>{
						beast::tcp_stream{ std::move(socket) },
							ctx_
					},
					server
				);
				//session->set_uri(std::format("{}:{}", config_loader::load_config()["host"].get<std::string>(), listen_port));
				session->start();
			}
			else {
				fail(ec, "accept");
			}
		}
	}

};

} // namespace leo

#endif // !SPARROW_LISTENER_HPP
