#pragma once
#ifndef SPARROW_ROUTE_SESSION_HPP
#define SPARROW_ROUTE_SESSION_HPP

#include "../base/websocket_session.hpp"

namespace leo {
namespace auth {
;
class auth_server;
class route_session
	: public websocket_session<route_session>
	, public std::enable_shared_from_this<route_session>
{
	auth_server& server_;

};
}
}

#endif // !SPARROW_ROUTE_SESSION_HPP