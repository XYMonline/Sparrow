#pragma once
#ifndef SPARROW_NET_HPP
#define SPARROW_NET_HPP

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/experimental/concurrent_channel.hpp>
#include <boost/asio/experimental/channel.hpp>

namespace net = boost::asio;
namespace ssl = net::ssl;
namespace expr = net::experimental;

using tcp = net::ip::tcp;
using executor_type = net::io_context::executor_type;
using executor_with_default = net::as_tuple_t<net::use_awaitable_t<executor_type>>::executor_with_default<executor_type>;
using boost::system::error_code;

#endif // !SPARROW_NET_HPP

