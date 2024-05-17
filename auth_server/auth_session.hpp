#pragma once
#ifndef SPARROW_AUTH_SESSION_HPP
#define SPARROW_AUTH_SESSION_HPP

#include "../base/websocket_session.hpp"
#include "../tools/cancellation_signals.hpp"

#include <print>

#include <random>

namespace leo {
namespace auth {
class auth_server;
class auth_session
        : public websocket_session<auth_session>
        , public std::enable_shared_from_this<auth_session>
{
	auth_server& server_;

public:
    auth_session(beast::ssl_stream<beast::tcp_stream> stream, auth_server& server);

    ~auth_session();

    net::awaitable<void> handle_messages_impl(std::shared_ptr<auth_session> self);

    static std::string server_name();
	cancellation_signals& signals();
};

}

}

#endif // !SPARROW_AUTH_SESSION_HPP

