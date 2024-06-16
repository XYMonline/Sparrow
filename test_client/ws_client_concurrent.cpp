#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>

#include <thread>
#include <print>
#include <vector>

namespace net = boost::asio;
namespace beast = boost::beast;
namespace ssl = boost::asio::ssl;
namespace websocket = beast::websocket;

ssl::context ssl_ctx(ssl::context::tlsv12_client);

std::atomic_bool stop = false;

auto connect_to_server(auto& ws, const std::string& host, const std::string& port, net::io_context& ioc) {
	boost::system::error_code ec;
	auto resolver = net::ip::tcp::resolver(ioc);
	auto results = resolver.resolve(host, port);
	//auto ws = websocket::stream<beast::ssl_stream<beast::tcp_stream>>(ioc, ssl_ctx);
	net::connect(ws.next_layer().next_layer().socket(), results.begin(), results.end());
	ws.next_layer().handshake(ssl::stream_base::client, ec);
	if (ec) {
		std::println("{}", ec.message());
		exit(1);
	}
	ws.handshake(host, "/", ec);
	if (ec) {
		std::println("{}", ec.message());
		exit(1);
	}
}

void task(auto& ws) {
	std::this_thread::sleep_for(std::chrono::months(1));
	
	ws.close(websocket::close_code::normal);
}

int main() {
	net::io_context ioc;
	ssl_ctx.set_default_verify_paths();
	ssl_ctx.set_verify_mode(ssl::verify_none);
	ssl_ctx.set_verify_callback(ssl::rfc2818_verification("localhost"));
	
	std::vector<std::jthread> threads;
	int interval = 5;
	int count = 10;

	for (int i = 0; i < count; ++i) {
		threads.emplace_back([&] {
			auto ws = websocket::stream<beast::ssl_stream<beast::tcp_stream>>(ioc, ssl_ctx);
			connect_to_server(ws, "localhost", "12300", ioc);
			ws.write(net::buffer(std::string("hello")));
			beast::flat_buffer buffer;
			ws.read(buffer);
			auto address = beast::buffers_to_string(buffer.data());
			auto host = address.substr(0, address.rfind(':'));
			auto port = address.substr(address.rfind(':') + 1);
			ws.close(websocket::close_code::normal);
			decltype(ws) new_ws(ioc, ssl_ctx);
			connect_to_server(new_ws, host, port, ioc);
			std::println("connected to {}:{}", host, port);
			task(new_ws);
			});
		
		std::this_thread::sleep_for(std::chrono::seconds(interval));

	}

	ioc.run();
	return 0;
}