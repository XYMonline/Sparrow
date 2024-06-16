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
	if (role_ == ssl::stream_base::client) {
		message_type::route_route msg;
		msg.set_category(message_type::ROUTE_JOIN);
		msg.set_uri(std::format("{}:{}", config_loader::load_config()["host"].get<std::string>(), server_.auth_port_));
		deliver(msg.SerializeAsString());
	}
}

void route_session::stop_impl() {
	//auto self = shared_from_this();
	server_.temp_remove<route_ptr>(uuid_);
	server_.perm_remove<route_ptr>(remote_uri_);
}

net::awaitable<void> route_session::handle_messages_impl(std::shared_ptr<route_session> self) {
	boost::system::error_code ec;
	auto token = net::redirect_error(net::deferred, ec);
	std::string message;
	message_type::route_route msg;
	message_type::route_auth auth_msg;

	while (ws_.is_open()) {
		message = co_await read_channel_.async_receive(token);
		if (!ec) [[likely]] {
			// handle message
			if (msg.ParseFromString(message)) [[likely]] {
				auto& _token = msg.token();
				auto& _uri = msg.uri();
				switch (msg.category()) {
				[[likely]] case message_type::UPDATE_LOAD:
				{
					auto& load_list = msg.load_list();
					for (auto& load : load_list) {
						server_.push_node_info(load, false); // to_route = false, 将消息推送到supervisor
					}
				}
					break;
				[[unlikely]] case message_type::SERVER_INFO:
					server_.perm_add(msg.uri(), shared_from_this());
					set_remote_uri(msg.uri());
					std::println("connect to route: {}", msg.uri());
					break;
				case message_type::ROUTE_JOIN:
					auth_msg.set_uri(msg.uri());
					auth_msg.set_category(message_type::ROUTE_JOIN);
					co_await server_.push_route_info(auth_msg.SerializeAsString());
					break;
				[[unlikely]] default:
					std::println("Debug message:\n{}", msg.DebugString());
					break;
				}
				auth_msg.Clear();
			}
			else [[unlikely]] {
				std::println("parse message failed: {}", message);
			}
			msg.Clear();
		}
		else {
			this->fail(ec, "handle_messages");
		}
	}
}

cancellation_signals& route_session::signals() {
	return server_.signals();
}

}
}