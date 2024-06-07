#include "supervisor_session.hpp"
#include "route_server.hpp"

#include "../tools/proto/supervisor_message.pb.h"

namespace leo {
namespace route {
;
supervisor_session::supervisor_session(beast::ssl_stream<beast::tcp_stream> stream, route_server& server)
	: websocket_session(std::move(stream))
	, server_(server) 
{
}

void supervisor_session::start_impl() {
	//server_.temp_add(shared_from_this());
}

void supervisor_session::stop_impl() {
	server_.temp_remove<supervisor_ptr>(uuid());
	server_.perm_remove<supervisor_ptr>(uuid());
}

net::awaitable<void> supervisor_session::handle_messages_impl(std::shared_ptr<supervisor_session> self) {
	boost::system::error_code ec;
	auto token = net::redirect_error(net::deferred, ec);
	std::string message;
	supr::route_supervisor msg;

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

cancellation_signals& supervisor_session::signals() {
	return server_.signals();
}

}
}
