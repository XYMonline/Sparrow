#pragma once
#ifndef SPARROW_ROUTE_BUSINESS_SESSION_HPP
#define SPARROW_ROUTE_BUSINESS_SESSION_HPP

#include "../base/websocket_session.hpp"

namespace leo {
namespace route {
;
class route_server;
/*
 * communicate with business server
 *
 */
class business_session
	: public websocket_session<business_session>
	, public std::enable_shared_from_this<business_session>
{
	route_server& server_;

public:
	business_session(beast::ssl_stream<beast::tcp_stream> stream, route_server& server);

	~business_session() = default;

	void start_impl();

	net::awaitable<void> handle_messages_impl(std::shared_ptr<business_session> self);

	static std::string server_name() {
		return "route_server business_session";
	}

	cancellation_signals& signals();
};
}
}

#endif // !SPARROW_ROUTE_BUSINESS_SESSION_HPP