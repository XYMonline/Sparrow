#pragma once
#ifndef SPARROW_BUSINESS_SESSION_HPP
#define SPARROW_BUSINESS_SESSION_HPP

#include "../base/websocket_session.hpp"

namespace leo {
namespace business {
;
class business_server;
 /*
  * communicate with route server
  *
  */
class route_session
	: public websocket_session<route_session>
	, public std::enable_shared_from_this<route_session>
{
	friend websocket_session<route_session>;

	business_server& server_;
	std::string route_uri_;

public:
	route_session(beast::ssl_stream<beast::tcp_stream> stream, business_server& server);


	void set_route_uri(std::string_view uri) { route_uri_ = uri; }

	static std::string server_name();
	cancellation_signals& signals();

private:
	void start_impl();
	void stop_impl();
	net::awaitable<void> handle_messages_impl(std::shared_ptr<route_session> self);
};
}
}

#endif // !SPARROW_ROUTE_BUSINESS_SESSION_HPP