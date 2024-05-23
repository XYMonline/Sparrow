#pragma once
#ifndef SERVER_CERTIFICATE_HPP
#define SERVER_CERTIFICATE_HPP

#include "../base/net.hpp"

#include <fstream>
#include <sstream>
#include <string>

namespace leo {
;
inline std::string read_file(const std::string& file_path) {
	std::ifstream file(file_path);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

inline void load_server_certificate(ssl::context& ctx, boost::system::error_code& ec, const std::string& cert_path, const std::string& key_path, const std::string& dh_path) {
	ctx.set_password_callback(
		[](std::size_t,
			boost::asio::ssl::context_base::password_purpose)
		{
			return "sparrow"; // use safe password in production
		});

	ctx.set_options(
		boost::asio::ssl::context::default_workarounds
		| boost::asio::ssl::context::no_sslv2
		| boost::asio::ssl::context::no_sslv3
		| boost::asio::ssl::context::no_tlsv1
		| boost::asio::ssl::context::single_dh_use);


	ctx.set_verify_mode(ssl::verify_peer); // production
	//ctx.set_verify_mode(ssl::verify_none); // test
	//ctx.set_default_verify_paths();

	std::string cert = read_file(cert_path);
	ctx.use_certificate_chain(
		boost::asio::buffer(cert.data(), cert.size()));
	ctx.load_verify_file(cert_path);

	std::string key = read_file(key_path);
	ctx.use_private_key(
		boost::asio::buffer(key.data(), key.size()),
		boost::asio::ssl::context::file_format::pem);

	std::string dh = read_file(dh_path);
	ctx.use_tmp_dh(
		boost::asio::buffer(dh.data(), dh.size()));
}
}


#endif // !SERVER_CERTIFICATE_HPP

