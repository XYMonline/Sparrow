#pragma once
#ifndef SERVER_CERTIFICATE_HPP
#define SERVER_CERTIFICATE_HPP

#include "../base/net.hpp"
#include "./service/logger.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

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

	std::string cert = cert_path.empty() ?  
		"-----BEGIN CERTIFICATE-----\n"
		"MIIDbTCCAlWgAwIBAgIUIgjuHEqy0v/BoKEhvQMqBhpPqfQwDQYJKoZIhvcNAQEL\n"
		"BQAwRTELMAkGA1UEBhMCQ04xEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoM\n"
		"GEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDAgFw0yNDA2MTMxMzMzMDlaGA8yMTI0\n"
		"MDUyMDEzMzMwOVowRTELMAkGA1UEBhMCQ04xEzARBgNVBAgMClNvbWUtU3RhdGUx\n"
		"ITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDCCASIwDQYJKoZIhvcN\n"
		"AQEBBQADggEPADCCAQoCggEBAIr89m9mmNUB74DU7Q/qz0sFDQNNi4ratUqihLm1\n"
		"n5w2sMomsfh7XOe6i+rXGAFNS1XnXNn63aS6qxB42ulvd3598GaKjHfdpbwdsQzM\n"
		"4X4caRv9V32czh5untZOxH42TQB5HBg0ryRuQcpp3PpdoEf9amiqVnVWMNoW341b\n"
		"54tjGMtX+vZh3jrwWrCBJQiguRGlMsS2dbJuQKgyOEW1wBOSZpDOEcC8F3AS+Qkt\n"
		"ODx7Uln20zF8C5MjB7vH2vFxTsGW456WPCQ4FnHZvPGHriuffhuo6x9MYA8ApQO0\n"
		"C8V0sOBpXJbZmp1H1mTa9CfZdRmk5qU9dmJqvHmBryiCjxUCAwEAAaNTMFEwHQYD\n"
		"VR0OBBYEFFe1bP0csMMZdCzKW93dTAoKVkywMB8GA1UdIwQYMBaAFFe1bP0csMMZ\n"
		"dCzKW93dTAoKVkywMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEB\n"
		"AH7+9bd8SrnlC4qwLaIJ4kifMAFFwf54vzCNa+wiBrJHfLJagofiCGh4Hz4fX5kd\n"
		"R6stlVgh1f9QCkvgEMmSly68NuGR4xmz8Fcx6kAZJj4ZyfR054lNFlBZebgaFI2C\n"
		"xhUAl8HvPeRxo6QZSTIvINh9M1bB9nQvGGDKCsGGD84Z1dKj7NJEW7wFIR2QDuOq\n"
		"iVAVfggNbNA0yUBFd9EJqscj8I89+MsCpR9KahVO672+qM3N82dtW4zsihP7Jn+/\n"
		"pGmD3HdKfz8sMkkq592IrPPi5mK/3yESNQ5hjSSENIY0gLkMb+ueOrBjBaQYsaCt\n"
		"+9zmoqgwUrza3wAVzGj3QyU=\n"
		"-----END CERTIFICATE-----\n"
		: read_file(cert_path);
	
	ctx.use_certificate_chain(
		boost::asio::buffer(cert.data(), cert.size()));

	if (cert_path.empty()) {
		std::ofstream file("default_cert.pem");
		file << cert;
		file.close();
		ctx.load_verify_file("default_cert.pem");
	}

	std::string key = key_path.empty() ?
		"-----BEGIN PRIVATE KEY-----\n"
		"MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCK/PZvZpjVAe+A\n"
		"1O0P6s9LBQ0DTYuK2rVKooS5tZ+cNrDKJrH4e1znuovq1xgBTUtV51zZ+t2kuqsQ\n"
		"eNrpb3d+ffBmiox33aW8HbEMzOF+HGkb/Vd9nM4ebp7WTsR+Nk0AeRwYNK8kbkHK\n"
		"adz6XaBH/WpoqlZ1VjDaFt+NW+eLYxjLV/r2Yd468FqwgSUIoLkRpTLEtnWybkCo\n"
		"MjhFtcATkmaQzhHAvBdwEvkJLTg8e1JZ9tMxfAuTIwe7x9rxcU7BluOeljwkOBZx\n"
		"2bzxh64rn34bqOsfTGAPAKUDtAvFdLDgaVyW2ZqdR9Zk2vQn2XUZpOalPXZiarx5\n"
		"ga8ogo8VAgMBAAECggEABO7ReEa16el1rKbh+wykas81FQbtivIUOR/nggfWMF8o\n"
		"iyQwcNdABuFyEdWsfm9V2yio/PuQXkZZx+3iJyLTpOOnYpxMO/zb4v9Sx1eGMUmk\n"
		"2V92eXjVP/NBI+4Lv0cCGKAzxRApnytinmJ/e0tTYvq1ih1xkByNA1CIgqXeY+4F\n"
		"D/WLBFfIbHUfmoJJI61kEh0Sd5YxoRemd47Kz7zI3rV/1yTw8WO2/6uq816W/ni2\n"
		"CfqlLOBsJon/HcdxiFFdg+99XZ/aUnSyud3ryRWoZWJae3aCj6BgW1vRVAwZS3hS\n"
		"h5eHMvj3HSb/dwccCnHsLi2PrgXXvc0bOLQzzA/+CQKBgQDCZaurPHOdEgqMozLF\n"
		"3OnZAPK1Y2zSLIqN4NBjsclUu/0wG5LYWOkwgpxsnc/Ghqon1MHCd/Xvx1BbGAGg\n"
		"e8M7pcif4SvFnoHRMwM520kr1zJK9tmVqUhvcKA5sntr8bZjAli+fK2hRnRngeIT\n"
		"tXZMtMrfQ66koboapXBvvMDg5wKBgQC3CEaCJXtEb2LTmG81Whn1B/ybPm+V9D/B\n"
		"1Y0nSOgrkBz3bsMHZt73SL1b4PW6O67FaLyITX0w0mkN9ShB+FUqE/2q2o7J6fl/\n"
		"S4igsUUrfNiiGsuW0lze0Wzj9UYfw7d7AGW3lMK/NuVqQuGlgOtX7cuEPDCiPvId\n"
		"jpKflZpEowKBgDqo82R4mWqMRyKpHGkYgHg7EQIrPmsthaHKdRegTmCDWaNqrqUa\n"
		"L0O0Jf+gn1II0M+0I8PK7yuruVPDqroCnqchH6kfCVZYvlOiURbkVPAlv8dfNckz\n"
		"i+gU1ESU1yl7hs3Zp23TsZE48WdKii3oVApRg+ZbRhfntYDU61jMXGeBAoGBAIYS\n"
		"U8OlvQPGK7gfYubL11O7w/fb5lULk2BjE4CBUUAH0pAU3FuhqRWufLa+tID0kvMb\n"
		"425chKWsmzOULycTxx6JgjkS26Ff+Ficdbpv3+EZFH657gPOmCTHd2IMGo/OB6rc\n"
		"AaZWG8mBZs9NqohXRQ0lrPxyMpXGUggAlbq8FULNAoGAHMIDLnaMLmmRLhaHCxjI\n"
		"AhDOt7Yj7GcdLEPNSbYuUOd4VUaVyWljORBg7KGs/blYqDn8chS6MZWNWJoCrvy2\n"
		"pBH7qbhIfaSndRhCqoZqi/mU8ps6TxP8HsG+Cs5tLC/5zzGZeeO2IIEL24rSH+kg\n"
		"kHLbTqtIbc6qAnfJizseCRI=\n"
		"-----END PRIVATE KEY-----\n"
		: read_file(key_path);
	ctx.use_private_key(
		boost::asio::buffer(key.data(), key.size()),
		boost::asio::ssl::context::file_format::pem);

	std::string dh = read_file(dh_path);
	ctx.use_tmp_dh(
		boost::asio::buffer(dh.data(), dh.size()));
}
}


#endif // !SERVER_CERTIFICATE_HPP

