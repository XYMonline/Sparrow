#pragma once
#ifndef SPARROW_LOGGER_HPP
#define SPARROW_LOGGER_HPP

#include <spdlog/spdlog.h>

#include <string>

namespace leo {

class logger {
	std::shared_ptr<spdlog::logger> debug_;
	std::shared_ptr<spdlog::logger> error_;
	std::shared_ptr<spdlog::logger> info_;
	std::shared_ptr<spdlog::logger> warn_;

public:
	logger(const std::string& type, 
		const std::string& level, 
		const std::string& path,
		size_t max_file_size = 104857600, 
		int max_files = 3, 
		int daily_hour = 0, 
		int daily_minute = 0);

	void debug(const std::string& msg);
	void error(const std::string& msg);
	void info(const std::string& msg);
	void warn(const std::string& msg);

	std::shared_ptr<spdlog::logger> raw_logger(const std::string& type);
};

} // namespace leo

#endif // !SPARROW_LOGGER_HPP
