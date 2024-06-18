#include "auth_session.hpp"
#include "route_server.hpp"
#include "business_session.hpp"

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
		if (!ec) [[likely]] {
			// handle message
			if (msg.ParseFromString(message)) [[likely]] {
				//std::println("{}", msg.DebugString());
				switch (msg.category()) {
				case message_type::UPDATE_LOAD:
				{
					auto& load = msg.server_load();

					// 同时将新的节点信息推送到route_info_channel_和node_info_channel_，用于在集群中传递新的节点信息和发送到监控管理器
					server_.push_node_info(load.SerializeAsString());
				}
				break;
				case message_type::REQUEST_ALLOCATE:
				{
					if (server_.business_online()) {
						std::string uri = msg.uri(), uid = msg.uid();
						std::function task = [uri, uid, self, this](business_ptr business) {
							message_type::route_auth msg;
							msg.set_uid(uid);
							auto remote_uri = business->remote_uri();
							msg.set_uri(remote_uri);
							msg.set_category(message_type::ALLOCATE_SUCCESS);
							server_.task_response(uri, msg.SerializeAsString());
							};
						server_.task_request(task);
					}
					else {
						msg.set_category(message_type::ALLOCATE_FAIL);
						server_.task_response(msg.uri(), msg.SerializeAsString());
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
				log().error("parse message failed, message: {}", message);
			}
		}
		else {
			this->fail(ec, "handle_messages");
		}
		msg.Clear();
	}
}

void auth_session::start_impl() {
	
}

void auth_session::stop_impl() {
	//auto self = shared_from_this();
	server_.temp_remove<auth_ptr>(uuid_);
	server_.perm_remove<auth_ptr>(remote_uri_);
}

cancellation_signals& auth_session::signals() {
	return server_.signals();
}

}
}