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

	std::string cert = cert.empty() ?  
		"-----BEGIN CERTIFICATE-----"
		"MIIDbTCCAlWgAwIBAgIUIgjuHEqy0v / BoKEhvQMqBhpPqfQwDQYJKoZIhvcNAQEL"
		"BQAwRTELMAkGA1UEBhMCQ04xEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoM"
		"GEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDAgFw0yNDA2MTMxMzMzMDlaGA8yMTI0"
		"MDUyMDEzMzMwOVowRTELMAkGA1UEBhMCQ04xEzARBgNVBAgMClNvbWUtU3RhdGUx"
		"ITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDCCASIwDQYJKoZIhvcN"
		"AQEBBQADggEPADCCAQoCggEBAIr89m9mmNUB74DU7Q / qz0sFDQNNi4ratUqihLm1"
		"n5w2sMomsfh7XOe6i + rXGAFNS1XnXNn63aS6qxB42ulvd3598GaKjHfdpbwdsQzM"
		"4X4caRv9V32czh5untZOxH42TQB5HBg0ryRuQcpp3PpdoEf9amiqVnVWMNoW341b"
		"54tjGMtX + vZh3jrwWrCBJQiguRGlMsS2dbJuQKgyOEW1wBOSZpDOEcC8F3AS + Qkt"
		"ODx7Uln20zF8C5MjB7vH2vFxTsGW456WPCQ4FnHZvPGHriuffhuo6x9MYA8ApQO0"
		"C8V0sOBpXJbZmp1H1mTa9CfZdRmk5qU9dmJqvHmBryiCjxUCAwEAAaNTMFEwHQYD"
		"VR0OBBYEFFe1bP0csMMZdCzKW93dTAoKVkywMB8GA1UdIwQYMBaAFFe1bP0csMMZ"
		"dCzKW93dTAoKVkywMA8GA1UdEwEB / wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEB"
		"AH7 + 9bd8SrnlC4qwLaIJ4kifMAFFwf54vzCNa + wiBrJHfLJagofiCGh4Hz4fX5kd"
		"R6stlVgh1f9QCkvgEMmSly68NuGR4xmz8Fcx6kAZJj4ZyfR054lNFlBZebgaFI2C"
		"xhUAl8HvPeRxo6QZSTIvINh9M1bB9nQvGGDKCsGGD84Z1dKj7NJEW7wFIR2QDuOq"
		"iVAVfggNbNA0yUBFd9EJqscj8I89 + MsCpR9KahVO672 + qM3N82dtW4zsihP7Jn + /"
		"pGmD3HdKfz8sMkkq592IrPPi5mK / 3yESNQ5hjSSENIY0gLkMb + ueOrBjBaQYsaCt"
		"+ 9zmoqgwUrza3wAVzGj3QyU ="
		"---- - END CERTIFICATE---- -"
		: read_file(cert_path);
	ctx.use_certificate_chain(
		boost::asio::buffer(cert.data(), cert.size()));
	ctx.load_verify_file(cert_path);

	std::string key = key.empty() ?
		"---- - BEGIN PRIVATE KEY---- -"
		"MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCK / PZvZpjVAe + A"
		"1O0P6s9LBQ0DTYuK2rVKooS5tZ + cNrDKJrH4e1znuovq1xgBTUtV51zZ + t2kuqsQ"
		"eNrpb3d + ffBmiox33aW8HbEMzOF + HGkb / Vd9nM4ebp7WTsR + Nk0AeRwYNK8kbkHK"
		"adz6XaBH / WpoqlZ1VjDaFt + NW + eLYxjLV / r2Yd468FqwgSUIoLkRpTLEtnWybkCo"
		"MjhFtcATkmaQzhHAvBdwEvkJLTg8e1JZ9tMxfAuTIwe7x9rxcU7BluOeljwkOBZx"
		"2bzxh64rn34bqOsfTGAPAKUDtAvFdLDgaVyW2ZqdR9Zk2vQn2XUZpOalPXZiarx5"
		"ga8ogo8VAgMBAAECggEABO7ReEa16el1rKbh + wykas81FQbtivIUOR / nggfWMF8o"
		"iyQwcNdABuFyEdWsfm9V2yio / PuQXkZZx + 3iJyLTpOOnYpxMO / zb4v9Sx1eGMUmk"
		"2V92eXjVP / NBI + 4Lv0cCGKAzxRApnytinmJ / e0tTYvq1ih1xkByNA1CIgqXeY + 4F"
		"D / WLBFfIbHUfmoJJI61kEh0Sd5YxoRemd47Kz7zI3rV / 1yTw8WO2 / 6uq816W / ni2"
		"CfqlLOBsJon / HcdxiFFdg + 99XZ / aUnSyud3ryRWoZWJae3aCj6BgW1vRVAwZS3hS"
		"h5eHMvj3HSb / dwccCnHsLi2PrgXXvc0bOLQzzA / +CQKBgQDCZaurPHOdEgqMozLF"
		"3OnZAPK1Y2zSLIqN4NBjsclUu / 0wG5LYWOkwgpxsnc / Ghqon1MHCd / Xvx1BbGAGg"
		"e8M7pcif4SvFnoHRMwM520kr1zJK9tmVqUhvcKA5sntr8bZjAli + fK2hRnRngeIT"
		"tXZMtMrfQ66koboapXBvvMDg5wKBgQC3CEaCJXtEb2LTmG81Whn1B / ybPm + V9D / B"
		"1Y0nSOgrkBz3bsMHZt73SL1b4PW6O67FaLyITX0w0mkN9ShB + FUqE / 2q2o7J6fl /"
		"S4igsUUrfNiiGsuW0lze0Wzj9UYfw7d7AGW3lMK / NuVqQuGlgOtX7cuEPDCiPvId"
		"jpKflZpEowKBgDqo82R4mWqMRyKpHGkYgHg7EQIrPmsthaHKdRegTmCDWaNqrqUa"
		"L0O0Jf + gn1II0M + 0I8PK7yuruVPDqroCnqchH6kfCVZYvlOiURbkVPAlv8dfNckz"
		"i + gU1ESU1yl7hs3Zp23TsZE48WdKii3oVApRg + ZbRhfntYDU61jMXGeBAoGBAIYS"
		"U8OlvQPGK7gfYubL11O7w / fb5lULk2BjE4CBUUAH0pAU3FuhqRWufLa + tID0kvMb"
		"425chKWsmzOULycTxx6JgjkS26Ff + Ficdbpv3 + EZFH657gPOmCTHd2IMGo / OB6rc"
		"AaZWG8mBZs9NqohXRQ0lrPxyMpXGUggAlbq8FULNAoGAHMIDLnaMLmmRLhaHCxjI"
		"AhDOt7Yj7GcdLEPNSbYuUOd4VUaVyWljORBg7KGs / blYqDn8chS6MZWNWJoCrvy2"
		"pBH7qbhIfaSndRhCqoZqi / mU8ps6TxP8HsG + Cs5tLC / 5zzGZeeO2IIEL24rSH + kg"
		"kHLbTqtIbc6qAnfJizseCRI ="
		"---- - END PRIVATE KEY---- -"
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

