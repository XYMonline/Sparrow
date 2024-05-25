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
	set_role(ssl::stream_base::client);
}

void route_session::start_impl() {
	message_type::route_business msg;
	msg.set_uri(uri_);
	msg.set_category(message_type::SERVER_INFO);
	deliver(msg.SerializeAsString());
}

net::awaitable<void> route_session::handle_messages_impl(std::shared_ptr<route_session> self) {
	boost::system::error_code ec;
	auto token = net::redirect_error(net::deferred, ec);
	std::string message;

	while (ws_.is_open()) {
		message = co_await read_channel_.async_receive(token);
		if (!ec) {
			// handle message
		}
		else {
			this->fail(ec, "handle_messages");
		}
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