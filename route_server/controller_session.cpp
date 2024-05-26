#include "controller_session.hpp"
#include "route_server.hpp"

#include "../tools/proto/controller_message.pb.h"

namespace leo {
namespace route {
;
controller_session::controller_session(beast::ssl_stream<beast::tcp_stream> stream, route_server& server)
	: websocket_session(std::move(stream))
	, server_(server) 
{
}

void controller_session::start_impl() {
	//server_.temp_add(shared_from_this());
}

void controller_session::stop_impl() {
	server_.temp_remove<controller_ptr>(uuid());
	server_.perm_remove<controller_ptr>(uuid());
}

net::awaitable<void> controller_session::handle_messages_impl(std::shared_ptr<controller_session> self) {
	boost::system::error_code ec;
	auto token = net::redirect_error(net::deferred, ec);
	std::string message;
	message_type::route_controller msg;

	while (ws_.is_open()) {
		message = co_await read_channel_.async_receive(token);
		if (!ec) [[likely]] {
			// handle message
			if (msg.ParseFromString(message)) [[likely]] {
				switch (msg.category()) {
				default:
					std::println("Debug message:\n{}", msg.DebugString());
					break;
				}
			}
			else {
				std::println("parse message failed: {}", message);
			}
		}
		else {
			this->fail(ec, "handle_messages");
		}
		msg.Clear();
	}
}

cancellation_signals& controller_session::signals() {
	return server_.signals();
}

}
}
