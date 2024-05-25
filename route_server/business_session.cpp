#include "business_session.hpp"
#include "route_server.hpp"

#include "../tools/proto/server_message.pb.h"

namespace leo {
namespace route {
;
business_session::business_session(beast::ssl_stream<beast::tcp_stream> stream, route_server& server)
	: websocket_session(std::move(stream))
	, server_(server) 
{
}

void business_session::start_impl() {
	server_.temp_add(shared_from_this());
}

net::awaitable<void> business_session::handle_messages_impl(std::shared_ptr<business_session> self) {
	boost::system::error_code ec;
	auto token = net::redirect_error(net::deferred, ec);
	std::string message;
	message_type::route_business msg;

	while (ws_.is_open()) {
		message = co_await read_channel_.async_receive(token);
		if (!ec) {
			// handle message
			if (msg.ParseFromString(message)) {
				//std::println("{}", msg.DebugString());
				switch (msg.category()) {
				case message_type::UPDATE_LOAD:
					break;
				case message_type::SERVER_INFO:
					server_.perm_add(msg.uri(), shared_from_this());
					break;
				}
			}
		}
		else {
			this->fail(ec, "handle_messages");
		}
		msg.Clear();
	}
}

cancellation_signals& business_session::signals() {
	return server_.signals();
}

}
}