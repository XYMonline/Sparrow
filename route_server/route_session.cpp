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
	// 如果是server，表示对方身份未确认，先加入临时列表。 如果是client，表示对方身份已确认，加入永久列表
	if (role_ == ssl::stream_base::server) {
		server_.temp_add(shared_from_this());
	}
	else {
		server_.perm_add(uri_, shared_from_this());
	}
}

void route_session::stop_impl() {
	server_.temp_remove<route_session>(uuid());
	server_.perm_remove<route_session>(uri_);
}

net::awaitable<void> route_session::handle_messages_impl(std::shared_ptr<route_session> self) {
	boost::system::error_code ec;
	auto token = net::redirect_error(net::deferred, ec);
	std::string message;
	message_type::route_route msg;

	while (ws_.is_open()) {
		message = co_await read_channel_.async_receive(token);
		if (!ec) {
			// handle message
			if (msg.ParseFromString(message)) {
				switch (msg.category()) {
				case message_type::SERVER_INFO:
					// 如果持有的token不是自己的uri，说明是一个新的连接
					if (msg.token() != uri_) {
						auto route = server_.make_route_session(msg.uri());
						if (!route) {
							std::println("route session create failed: {}", msg.uri());
						}
						else {
							route->set_uri(msg.uri());
							route->set_role(ssl::stream_base::client);
							route->start();
						}
					}
					server_.perm_add(msg.uri(), shared_from_this());
					break;
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

cancellation_signals& route_session::signals() {
	return server_.signals();
}

}
}