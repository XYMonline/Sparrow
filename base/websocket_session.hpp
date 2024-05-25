#pragma once
#ifndef SPARROW_WEBSOCKET_SESSION_HPP
#define SPARROW_WEBSOCKET_SESSION_HPP

#include "net.hpp"
#include "beast.hpp"
#include "logger.hpp"

#include "../tools/tool_func.hpp"
#include "../tools/cancellation_signals.hpp"

#include <print>

namespace leo {
;

/*
 * derived class must implement the following functions:
 * - net::awaitable<void> handle_messages_impl : fetch message from read_channel_ and process it, then send it to write_channel_
 * - std::string server_name : return the server name
 * - cancellation_signals& signals : return the signals from the server
 * - void start_impl : start the session
 */
template<typename Derived>
class websocket_session {
	Derived& derived() {
		return static_cast<Derived&>(*this);
	}
protected:
	websocket_stream ws_;
	expr::concurrent_channel<void(boost::system::error_code, std::string)> read_channel_;
	expr::concurrent_channel<void(boost::system::error_code, std::string)> write_channel_;
	expr::concurrent_channel<void()> write_lock_; // fix, it will be used in writer and handle_messages

	std::string uuid_;
	std::string uri_;

	ssl::stream_base::handshake_type role_{ ssl::stream_base::server };

public:
	websocket_session(beast::ssl_stream<beast::tcp_stream>&& stream)
		: ws_(std::move(stream))
		, read_channel_{ ws_.get_executor(), 4096 } // a big buffer
		, write_channel_{ ws_.get_executor(), 4096 }
		, write_lock_{ ws_.get_executor(), 1 }  // 1 for single writer
		, uuid_{ uuid_gen()}
	{
		ws_.set_option(
			websocket::stream_base::timeout::suggested(
				beast::role_type::server));

		ws_.set_option(websocket::stream_base::decorator(
			[this](websocket::response_type& res) {
				res.set(http::field::server, // 子类重新设置服务器名称
				derived().server_name());
			}));
	}

	~websocket_session() {
		std::println("destroy {} uuid: {}", derived().server_name(), derived().uuid());
	}

	std::string uuid() const {
		return uuid_;
	}

	void set_uri(const std::string& uri) {
		uri_ = uri;
	}

	// pass message to the write queue
	void deliver(const std::string& message) {
		auto self = derived().shared_from_this();
		net::co_spawn(ws_.get_executor(), [this, self, message]() -> net::awaitable<void> {
			boost::system::error_code ec;
			co_await write_channel_.async_send({}, message, net::redirect_error(net::use_awaitable, ec));
			if (ec) {
				self->fail(ec, "deliver");
			}
			},
			net::detached
		);
	}

	void start() {
		auto self = derived().shared_from_this();
		net::co_spawn(ws_.get_executor(), [self]() mutable {
			return self->handshake(self);
			},
			net::detached
		);
	}

	void stop() {
		boost::system::error_code ec;
		ws_.close(websocket::close_code::normal, ec);
		read_channel_.cancel();
		write_channel_.cancel();
		write_lock_.cancel();

		// save channel data

		// complete and close channel
		read_channel_.close();
		write_channel_.close();
		write_lock_.close();

		std::println("shutdown and close socket");
		derived().stop_impl();
	}

	void set_role(ssl::stream_base::handshake_type role) {
		role_ = role;
	}

protected:
	void fail(boost::system::error_code ec, char const* what, const char* who = "") {
		if (ec == net::error::operation_aborted || !ec || ec == expr::error::channel_cancelled) {
			return;
		}
		if (ec == websocket::error::closed || ec == net::error::eof || ec == net::error::connection_aborted || ec == net::error::connection_reset) {
			stop();
			return;
		}
		std::println("{}: {} code: {} {}", what, ec.message(), ec.value(), who);
	}

private:
	net::awaitable<void> handshake(auto self) {
		boost::system::error_code ec;
		auto token = net::redirect_error(net::use_awaitable, ec);

		// Perform the SSL handshake
		co_await ws_.next_layer().async_handshake(role_, token);
		if (ec) {
			this->fail(ec, "handshake");
			co_return;
		}

		// derived class is server or client
		if (role_ == ssl::stream_base::server) {
			co_await ws_.async_accept(token);
			if (ec) {
				this->fail(ec, "handshake_accept");
				co_return;
			}
		}
		else {
			co_await ws_.async_handshake(uri_, "/", token);
			if (ec) {
				this->fail(ec, "handshake_connect");
				co_return;
			}
		}
		
		net::co_spawn(
			ws_.get_executor(),
			this->reader(self),
			net::bind_cancellation_slot(derived().signals().slot(), net::detached)
		);
		net::co_spawn(
			ws_.get_executor(),
			this->writer(self),
			net::bind_cancellation_slot(derived().signals().slot(), net::detached)
		);
		net::co_spawn(
			ws_.get_executor(),
			this->handle_messages(self),
			net::bind_cancellation_slot(derived().signals().slot(), net::detached)
		);

		derived().start_impl();
	}

	net::awaitable<void> reader(auto self) {
		boost::system::error_code ec;
		auto token = net::redirect_error(net::deferred, ec);
		beast::flat_buffer buffer;
		buffer.reserve(4096);
		size_t n{ 0 };
		while (ws_.is_open()) {
			n = co_await ws_.async_read(buffer, token);// 解析消息
			if (!ec) {
				//std::println("message: {}", beast::buffers_to_string(buffer.data()));
				co_await read_channel_.async_send({}, beast::buffers_to_string(buffer.data()), token);
				buffer.consume(n);
			}
			else {
				this->fail(ec, "reader", __func__);
			}
		}
	}

	net::awaitable<void> handle_messages(auto self) {
		return derived().handle_messages_impl(self);
	}

	net::awaitable<void> writer(auto self) {
		boost::system::error_code ec;
		auto token = net::redirect_error(net::deferred, ec);
		std::string message;
		message.reserve(4096);
		while (ws_.is_open()) {
			message = co_await write_channel_.async_receive(token);
			if (!ec) {
				co_await write_lock_.async_send(token);
				if (ec) this->fail(ec, "write_lock send", __func__);

				co_await ws_.async_write(net::buffer(message), token);
				if (ec) this->fail(ec, "ws write", __func__);

				message.clear();
				write_lock_.try_receive([](auto...) {});
			}
			else {
				this->fail(ec, "writer_channel receive");
			}
		}
	}
};


}

#endif // !SPARROW_WEBSOCKET_SESSION_HPP
