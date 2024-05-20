#pragma once
#ifndef SPARROW_CONTROLER_SESSION_HPP
#define SPARROW_ROUTE_BUSINESS_SESSION_HPP

#include "../base/websocket_session.hpp"

namespace leo {
namespace route {
;
class route_server;
/*
 * communicate with controler
 *
 */
class controler_session
	: public websocket_session<controler_session>
	, public std::enable_shared_from_this<controler_session>
{
	route_server& server_;

public:
	controler_session(beast::ssl_stream<beast::tcp_stream> stream, route_server& server);

	~controler_session();

	net::awaitable<void> handle_messages_impl(std::shared_ptr<controler_session> self);

	static std::string server_name() {
		return "route_server";
	}

	cancellation_signals& signals();
};
}
}

#endif // !SPARROW_CONTROLER_SESSION_HPP