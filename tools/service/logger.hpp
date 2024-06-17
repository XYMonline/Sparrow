#pragma once
#ifndef SPARROW_LOGGER_HPP
#define SPARROW_LOGGER_HPP

#include <spdlog/spdlog.h>

#include <string>

namespace leo {

class logger {
	std::shared_ptr<spdlog::logger> log_;

public:
	logger(const std::string& type, 
		const std::string& level, 
		const std::string& path,
		size_t max_file_size = 104857600, 
		int max_files = 3, 
		int daily_hour = 0, 
		int daily_minute = 0);

	template<typename... Args> 
	void debug(spdlog::format_string_t<Args...> fmt, Args&& ...args) {
		log_->debug(fmt, std::forward<Args>(args)...);
	}

	template<typename... Args> 
	void error(spdlog::format_string_t<Args...> fmt, Args&& ...args) {
		log_->error(fmt, std::forward<Args>(args)...);
	}

	template<typename... Args> 
	void info(spdlog::format_string_t<Args...> fmt, Args&& ...args) {
		log_->info(fmt, std::forward<Args>(args)...);
	}

	template<typename... Args> 
	void warn(spdlog::format_string_t<Args...> fmt, Args&& ...args) {
		log_->warn(fmt, std::forward<Args>(args)...);
	}

	std::shared_ptr<spdlog::logger> raw_logger(const std::string& type);
};

} // namespace leo

#endif // !SPARROW_LOGGER_HPP
