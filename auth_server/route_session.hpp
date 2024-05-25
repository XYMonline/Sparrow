#pragma once
#ifndef SPARROW_AUTH_ROUTE_SESSION_HPP
#define SPARROW_AUTH_ROUTE_SESSION_HPP

#include "../base/websocket_session.hpp"

namespace leo {
namespace auth {
;
class auth_server;
/*
 * communicate with route server
 *
 */
class route_session
	: public websocket_session<route_session>
	, public std::enable_shared_from_this<route_session>
{
	auth_server& server_;

public:
	route_session(beast::ssl_stream<beast::tcp_stream> stream, auth_server& server);

	void start_impl();
	void stop_impl();

	net::awaitable<void> handle_messages_impl(std::shared_ptr<route_session> self);

	static std::string server_name();

	cancellation_signals& signals();
};
}
}

#endif // !SPARROW_AUTH_ROUTE_SESSION_HPP