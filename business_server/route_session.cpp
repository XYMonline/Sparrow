#include "route_session.hpp"
#include "business_server.hpp"

#include "../tools/proto/server_message.pb.h"

namespace leo {
namespace business {
;
route_session::route_session(beast::ssl_stream<beast::tcp_stream> stream, business_server& server) 
	: websocket_session(std::move(stream))
	, server_(server) 
{
	as_client();
}

void route_session::start_impl() {
	message_type::route_business msg;
	msg.set_uri(local_uri_);
	msg.set_category(message_type::SERVER_INFO);
	deliver(msg.SerializeAsString());
}

void route_session::stop_impl() {
	server_.temp_remove<route_session>(uuid());
	server_.perm_remove<route_session>(route_uri_);

	bool res = server_.connect_route();
	if (!res) {
		std::println("connect to route failed");
		server_.stop();
		std::exit(1);
	}
}

net::awaitable<void> route_session::handle_messages_impl(std::shared_ptr<route_session> self) {
	boost::system::error_code ec;
	auto token = net::redirect_error(net::deferred, ec);
	std::string message;
	message_type::route_business msg;

	while (ws_.is_open()) {
		message = co_await read_channel_.async_receive(token);
		if (!ec) [[likely]] {
			// handle message
			if (msg.ParseFromString(message)) {
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

std::string route_session::server_name() {
	return "business_server route_session";
}

cancellation_signals& route_session::signals() {
	return server_.signals();
}

}
}