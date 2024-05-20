#include "controller_session.hpp"
#include "route_server.hpp"

namespace leo {
namespace route {
;
controller_session::controller_session(beast::ssl_stream<beast::tcp_stream> stream, route_server& server)
	: websocket_session(std::move(stream))
	, server_(server) 
{
}

controller_session::~controller_session() {
}

net::awaitable<void> controller_session::handle_messages_impl(std::shared_ptr<controller_session> self) {
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

cancellation_signals& controller_session::signals() {
	return server_.signals();
}

}
}
