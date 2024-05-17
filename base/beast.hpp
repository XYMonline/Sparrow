#pragma once
#ifndef SPARROW_BEAST_HPP
#define SPARROW_BEAST_HPP

#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;

//using websocket_stream = websocket::stream<
//      typename beast::tcp_stream::rebind_executor<
//      typename net::use_awaitable_t<>::executor_with_default<net::any_io_executor>>::other>;
using websocket_stream = websocket::stream<beast::ssl_stream<beast::tcp_stream>>;

#endif // !SPARROW_BEAST_HPP

