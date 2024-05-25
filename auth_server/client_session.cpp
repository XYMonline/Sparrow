#include "client_session.hpp"
#include "auth_server.hpp"

#include "../tools/proto/client_message.pb.h"

namespace leo {
namespace auth {
;

client_session::client_session(beast::ssl_stream<beast::tcp_stream> stream, auth_server& server)
	: websocket_session{ std::move(stream) }
	, server_{ server }
{
	
}

void client_session::start_impl() {
	server_.temp_add(shared_from_this());
}

void client_session::stop_impl() {
	server_.temp_remove<client_session>(uuid());
	server_.perm_remove<client_session>(uuid());
}

net::awaitable<void> client_session::handle_messages_impl(std::shared_ptr<client_session> self) {
	boost::system::error_code ec;
	auto token = net::redirect_error(net::deferred, ec);
	std::string message;

	std::random_device rd;

	while (ws_.is_open()) {
		message = co_await read_channel_.async_receive(token);
		if (!ec) {
			//echo
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

std::string client_session::server_name() {
	return "Sparrow auth_server";
}

cancellation_signals& client_session::signals() {
	return server_.signals();
}

}
}
