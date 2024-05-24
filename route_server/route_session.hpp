#pragma once
#ifndef SPARROW_ROUTE_ROUTE_SESSION_HPP
#define SPARROW_ROUTE_ROUTE_SESSION_HPP

#include "../base/websocket_session.hpp"

namespace leo {
namespace route {
;
class route_server;
/*
 * communicate with other route server
 */
class route_session
	: public websocket_session<route_session>
	, public std::enable_shared_from_this<route_session>
{
	route_server& server_;

public:
	route_session(beast::ssl_stream<beast::tcp_stream> stream, route_server& server);

	~route_session() = default;

	void start_impl();
	void stop_impl() {}

	net::awaitable<void> handle_messages_impl(std::shared_ptr<route_session> self);

	static std::string server_name() {
		return "route_server route_session";
	}

	cancellation_signals& signals();
};
}
}

#endif // !SPARROW_ROUTE_ROUTE_SESSION_HPP