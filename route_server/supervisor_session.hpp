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
class supervisor_session
	: public websocket_session<supervisor_session>
	, public std::enable_shared_from_this<supervisor_session>
{
	friend websocket_session<supervisor_session>;

	route_server& server_;

public:
	supervisor_session(beast::ssl_stream<beast::tcp_stream> stream, route_server& server);

	~supervisor_session() = default;

	static std::string server_name() {
		return "route_server supervisor_session";
	}

	cancellation_signals& signals();

private:
	void start_impl();
	void stop_impl();
	net::awaitable<void> handle_messages_impl(std::shared_ptr<supervisor_session> self);
};
}
}

#endif // !SPARROW_CONTROLER_SESSION_HPP