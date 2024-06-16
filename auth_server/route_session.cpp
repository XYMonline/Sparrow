#include "route_session.hpp"
#include "auth_server.hpp"

#include "../tools/proto/server_message.pb.h"

namespace leo {
namespace auth {
;

route_session::route_session(beast::ssl_stream<beast::tcp_stream> stream, auth_server& server)
	: websocket_session{ std::move(stream) }
	, server_{ server } 
{
	//set_role(ssl::stream_base::client);
	as_client();
}

void route_session::start_impl() {
	message_type::route_auth msg;
	msg.set_uri(local_uri_);
	msg.set_category(message_type::SERVER_INFO);
	deliver(msg.SerializeAsString());
}

void route_session::stop_impl() {
	server_.perm_remove<route_ptr>(remote_uri_);
	server_.check_routes();
}

net::awaitable<void> route_session::handle_messages_impl(std::shared_ptr<route_session> self) {
	boost::system::error_code ec;
	auto token = net::redirect_error(net::deferred, ec);
	std::string message;
	message_type::route_auth msg;

	int tick = 0;

	while (ws_.is_open()) {
		message = co_await read_channel_.async_receive(token);
		if (!ec) [[likely]] {
			// handle message
			if (msg.ParseFromString(message)) {
				switch (msg.category()) {
				case message_type::ROUTE_JOIN:
					server_.make_route_session(msg.uri());
					break;
				case message_type::ALLOCATE_SUCCESS:
					server_.task_response(msg.uid(), msg.uri());
					break;
				case message_type::ALLOCATE_FAIL:
					server_.task_response(msg.uid(), "/allocate fail");
					break;
				[[likely]] case message_type::UPDATE_LOAD:
					load_ = msg.server_load().session_count();
					++tick;
					if (tick % 10 == 0) {
						std::println("{} load: {}", remote_uri_, load_.load());
					}
					break;
				[[unlikely]] default:
					std::println("Debug message:\n{} {}", msg.DebugString(), msg.SerializeAsString());
					break;
				}
			}
			else {
				std::println("parse message failed: {}", message);
			}
			msg.Clear();
		}
		else {
			this->fail(ec, "handle_messages");
		}
	}
}

std::string route_session::server_name() {
	return "auth_server route_session";
}

cancellation_signals& route_session::signals() {
	return server_.signals();
}

}
}