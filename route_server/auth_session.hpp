#pragma once
#ifndef SPARROW_ROUTE_AUTH_SESSION_HPP
#define SPARROW_ROUTE_AUTH_SESSION_HPP

#include "../base/websocket_session.hpp"

namespace leo {
namespace route {
;
class route_server;
/*
 * communicate with auth server
 */
class auth_session
	: public websocket_session<auth_session>
	, public std::enable_shared_from_this<auth_session>
{
	route_server& server_;

public:
	auth_session(beast::ssl_stream<beast::tcp_stream> stream, route_server& server);

	~auth_session() = default;

	net::awaitable<void> handle_messages_impl(std::shared_ptr<auth_session> self);

	void start_impl();
	void stop_impl() {}

	static std::string server_name() {
		return "route_server auth_session";
	}

	cancellation_signals& signals();
};
}
}

#endif // !SPARROW_ROUTE_AUTH_SESSION_HPP