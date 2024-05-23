#include "tool_func.hpp"

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <openssl/evp.h>

#include <chrono>

namespace leo {
;
std::string uuid_gen() {
	static auto generator = boost::uuids::random_generator();
	return boost::uuids::to_string(generator());
}

int64_t get_timestamp() {
	auto now = std::chrono::system_clock::now();
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
	return seconds.count();
}

std::string format_timestamp(int64_t timestamp) {
	std::time_t t = static_cast<std::time_t>(timestamp);
	std::tm timeInfo;

#if defined(_MSC_VER) && !defined(__clang__) // Microsoft Visual C++ 编译器
	localtime_s(&timeInfo, &timestamp);
#elif defined(__GNUC__) || defined(__clang__) // GNU 编译器和 Clang
	std::tm* result = std::localtime(&t);
	if (result) {
		timeInfo = *result;
	}
	else {
		// 错误处理
		return "Invalid Timestamp";
	}
#else
#error "Unsupported Compiler"
#endif

	char buffer[20];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);

	return buffer;
}

std::tuple<std::string, std::string> uri2host_port(const std::string& uri) {
	auto pos = uri.rfind(':');
	if (pos != std::string::npos) {
		return std::make_tuple(uri.substr(0, pos), uri.substr(pos + 1));
	}
	return std::tuple<std::string, std::string>();
}

std::string calculateSHA512(const std::string& data) {
	const EVP_MD* md = EVP_sha512();
	unsigned char hash[EVP_MAX_MD_SIZE];
	unsigned int hashLen;
	char outputBuffer[EVP_MAX_MD_SIZE * 2 + 1];

	EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
	EVP_DigestInit_ex(mdctx, md, NULL);
	EVP_DigestUpdate(mdctx, data.c_str(), data.size());
	EVP_DigestFinal_ex(mdctx, hash, &hashLen);
	EVP_MD_CTX_free(mdctx);

	for (unsigned int i = 0; i < hashLen; ++i) {
		std::snprintf(outputBuffer + (i * 2), sizeof(outputBuffer) - (i * 2), "%02x", hash[i]);
	}
	outputBuffer[hashLen * 2] = 0;

	return std::string(outputBuffer);
}

}