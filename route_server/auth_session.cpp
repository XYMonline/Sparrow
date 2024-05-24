#include "auth_session.hpp"
#include "route_server.hpp"

#include "../tools/proto/server_message.pb.h"

namespace leo {
namespace route {
;
auth_session::auth_session(beast::ssl_stream<beast::tcp_stream> stream, route_server& server)
	: websocket_session{ std::move(stream) }
	, server_{ server } 
{
}

net::awaitable<void> auth_session::handle_messages_impl(std::shared_ptr<auth_session> self) {
	boost::system::error_code ec;
	auto token = net::redirect_error(net::deferred, ec);
	std::string message; // message_buff
	message_type::route_auth msg; // deserialized message

	while (ws_.is_open()) {
		message = co_await read_channel_.async_receive(token);
		if (!ec) {
			// handle message
			if (msg.ParseFromString(message)) {
				//std::println("{}", msg.DebugString());
				switch (msg.category()) {
				case message_type::REQUEST_ALLOCATE:
					std::println("request allocate");
					break;
				case message_type::SERVER_INFO:
					server_.perm_add(msg.uri(), shared_from_this());
					break;
				}

				msg.Clear();
			}
			else {
				std::println("parse message failed, message: {}", message);
			}
		}
		else {
			this->fail(ec, "handle_messages");
		}
	}
}

void auth_session::start_impl() {
	server_.temp_add<auth_ptr>(shared_from_this());
}

cancellation_signals& auth_session::signals() {
	return server_.signals();
}

}
}