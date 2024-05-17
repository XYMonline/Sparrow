#include "auth_session.hpp"

#include "auth_server.hpp"

namespace leo {
namespace auth {
;

auth_session::auth_session(beast::ssl_stream<beast::tcp_stream> stream, auth_server& server)
	: websocket_session{ std::move(stream) }
	, server_{ server }
{

}

auth_session::~auth_session() {
	std::println("session destructed");
}

net::awaitable<void> auth_session::handle_messages_impl(std::shared_ptr<auth_session> self) {
	boost::system::error_code ec;
	auto token = net::redirect_error(net::deferred, ec);
	std::string message;

	std::random_device rd;

	while (ws_.is_open()) {
		message = co_await read_channel_.async_receive(token);
		if (!ec) {
			co_await write_channel_.async_send({}, message, token);
			if (ec) {
				this->fail(ec, "handle_messages");
			}
			if (rd() % 2) {
				co_await write_lock_.async_send(token);
				co_await ws_.async_write(net::buffer("SPECIAL_STRING"), token);
				write_lock_.try_receive([](auto...) {});
			}
		}
		else {
			this->fail(ec, "handle_messages");
		}
	}
}

std::string auth_session::server_name() {
	return "Sparrow auth_server";
}

cancellation_signals& auth_session::signals() {
	return server_.signals();
}

}
}
