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
	business_server& server_;

public:
	route_session(beast::ssl_stream<beast::tcp_stream> stream, business_server& server);

	void start_impl();

	net::awaitable<void> handle_messages_impl(std::shared_ptr<route_session> self);

	static std::string server_name();

	cancellation_signals& signals();
};
}
}

#endif // !SPARROW_ROUTE_BUSINESS_SESSION_HPP