#include "client_session.hpp"
#include "business_server.hpp"

#include "../tools/proto/client_message.pb.h"

namespace leo {
namespace business {
;

client_session::client_session(beast::ssl_stream<beast::tcp_stream> stream, business_server& server)
	: websocket_session{ std::move(stream) }
	, server_{ server }
{

}

net::awaitable<void> client_session::handle_messages_impl(std::shared_ptr<client_session> self) {
	boost::system::error_code ec;
	auto token = net::redirect_error(net::deferred, ec);
	std::string message;

	while (ws_.is_open()) {
		message = co_await read_channel_.async_receive(token);
		if (!ec) {
			//handle message
		}
		else {
			this->fail(ec, "handle_messages");
		}
	}
}

std::string client_session::server_name() {
	return "Sparrow business_server";
}

cancellation_signals& client_session::signals() {
	return server_.signals();
}

}
}
