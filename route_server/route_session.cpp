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
	server_.temp_add(shared_from_this());
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

cancellation_signals& route_session::signals() {
	return server_.signals();
}

}
}