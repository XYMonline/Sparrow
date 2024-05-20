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

business_session::~business_session() {
}

net::awaitable<void> business_session::handle_messages_impl(std::shared_ptr<business_session> self) {
	boost::system::error_code ec;
	auto token = net::redirect_error(net::deferred, ec);
	std::string message;

	while (ws_.is_open()) {
		message = co_await read_channel_.async_receive(token);
		if (!ec) {
			co_await write_channel_.async_send({}, message, token);
			if (ec) {
				this->fail(ec, "handle_messages");
			}
			// handle message
		}
		else {
			this->fail(ec, "handle_messages");
		}
	}
}

cancellation_signals& business_session::signals() {
	return server_.signals();
}

}
}