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
class controller_session
	: public websocket_session<controller_session>
	, public std::enable_shared_from_this<controller_session>
{
	route_server& server_;

public:
	controller_session(beast::ssl_stream<beast::tcp_stream> stream, route_server& server);

	~controller_session() = default;

	void start_impl();

	net::awaitable<void> handle_messages_impl(std::shared_ptr<controller_session> self);

	static std::string server_name() {
		return "route_server controller_session";
	}

	cancellation_signals& signals();
};
}
}

#endif // !SPARROW_CONTROLER_SESSION_HPP