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
 */
class business_session
	: public websocket_session<business_session>
	, public std::enable_shared_from_this<business_session>
{
	friend websocket_session<business_session>;

	using load_type = std::atomic_int;

	route_server& server_;
	load_type load_{ 0 };

public:
	business_session(beast::ssl_stream<beast::tcp_stream> stream, route_server& server);

	~business_session() = default;

	const load_type& load() const {
		return load_;
	}

	static std::string server_name() {
		return "route_server business_session";
	}

	cancellation_signals& signals();

private:
	void start_impl();
	void stop_impl();
	net::awaitable<void> handle_messages_impl(std::shared_ptr<business_session> self);
};
}
}

#endif // !SPARROW_ROUTE_BUSINESS_SESSION_HPP