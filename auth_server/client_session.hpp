#pragma once
#ifndef SPARROW_AUTH_CLIENT_SESSION_HPP
#define SPARROW_AUTH_CLIENT_SESSION_HPP

#include "../base/websocket_session.hpp"
#include "../tools/cancellation_signals.hpp"

#include <print>

#include <random>

namespace leo {
namespace auth {
class auth_server;

/*
 * handle messages from client
 */
class client_session
        : public websocket_session<client_session>
        , public std::enable_shared_from_this<client_session>
{
	auth_server& server_;

public:
    client_session(beast::ssl_stream<beast::tcp_stream> stream, auth_server& server);

	void start_impl();
    void stop_impl() {}

    net::awaitable<void> handle_messages_impl(std::shared_ptr<client_session> self);

    static std::string server_name();
	cancellation_signals& signals();
};

}

}

#endif // !SPARROW_AUTH_CLIENT_SESSION_HPP

