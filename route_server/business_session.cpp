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
	
}

void business_session::stop_impl() {
	server_.temp_remove<business_ptr>(uuid());
	server_.perm_remove<business_ptr>(remote_uri_);
}

net::awaitable<void> business_session::handle_messages_impl(std::shared_ptr<business_session> self) {
	boost::system::error_code ec;
	auto token = net::redirect_error(net::deferred, ec);
	std::string message;
	message_type::route_business msg;

	while (ws_.is_open()) {
		message = co_await read_channel_.async_receive(token);
		if (!ec) [[likely]] {
			// handle message
			if (msg.ParseFromString(message)) [[likely]] {
				std::println("{}", msg.DebugString());
				switch (msg.category()) {
				case message_type::UPDATE_LOAD:
				{
					auto& load = msg.server_load();
					server_.session_total_inc(load.session_increase());

					// 同时将新的节点信息推送到route_info_channel_和node_info_channel_，用于在集群中传递新的节点信息和发送到监控管理器
					server_.push_node_info(load.SerializeAsString());
				}
					break;
				[[unlikely]] case message_type::SERVER_INFO:
					server_.perm_add(msg.uri(), shared_from_this());
					set_remote_uri(msg.uri());
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