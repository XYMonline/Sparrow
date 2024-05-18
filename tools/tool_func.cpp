#include "tool_func.hpp"

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

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

}