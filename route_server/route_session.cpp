#include "route_session.hpp"
#include "route_server.hpp"

#include "../tools/proto/server_message.pb.h"

namespace leo {
namespace route {
;

route_session::route_session(beast::ssl_stream<beast::tcp_stream> stream, route_server& server) 
	: websocket_session(std::move(stream))
	, server_(server) 
{
}

void route_session::start_impl() {
	//if (role_ == ssl::stream_base::client) {
	//	message_type::route_route msg;
	//	msg.set_category(message_type::SERVER_INFO);
	//	msg.set_uri(uri_);
	//	deliver(msg.SerializeAsString());
	//}
}

void route_session::stop_impl() {
	server_.temp_remove<route_ptr>(uuid());
	server_.perm_remove<route_ptr>(uri_);
}

net::awaitable<void> route_session::handle_messages_impl(std::shared_ptr<route_session> self) {
	boost::system::error_code ec;
	auto token = net::redirect_error(net::deferred, ec);
	std::string message;
	message_type::route_route msg;

	while (ws_.is_open()) {
		message = co_await read_channel_.async_receive(token);
		if (!ec) [[likely]] {
			// handle message
			if (msg.ParseFromString(message)) [[likely]] {
				auto& _token = msg.token();
				auto& _uri = msg.uri();
				switch (msg.category()) {
				[[unlikely]] case message_type::SERVER_INFO:
					server_.perm_add(msg.uri(), shared_from_this());
					set_uri(msg.uri());
					std::println("connect to route: {}", msg.uri());
					break;
				[[unlikely]] default:
					std::println("Debug message:\n{}", msg.DebugString());
					break;
				}
			}
			else [[unlikely]] {
				std::println("parse message failed: {}", message);
			}
		}
		else {
			this->fail(ec, "handle_messages");
		}
		msg.Clear();
	}
}

cancellation_signals& route_session::signals() {
	return server_.signals();
}

}
}