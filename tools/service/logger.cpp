#include "logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>

#include <print>
#include <filesystem>

namespace fs = std::filesystem;

namespace leo {
;
logger::logger(const std::string& type, 
	const std::string& level,
	const std::string& path,
	size_t max_file_size, 
	int max_files, 
	int daily_hour, 
	int daily_minute)
{
	if (!fs::exists(path)) {
		fs::create_directory(path);
	}

	fs::path log_path{ path };
	auto debug_path{ log_path / "debug.log" };
	auto error_path{ log_path / "error.log" };
	auto info_path{ log_path / "info.log" };
	auto warn_path{ log_path / "warn.log" };

	if (type == "console") {
		debug_ = spdlog::stdout_color_mt("debug");
		error_ = spdlog::stdout_color_mt("error");
		info_ = spdlog::stdout_color_mt("info");
		warn_ = spdlog::stdout_color_mt("warn");
	}
	else if (type == "basic_file") {
		debug_ = spdlog::basic_logger_mt("debug", debug_path.string());
		error_ = spdlog::basic_logger_mt("error", error_path.string());
		info_ = spdlog::basic_logger_mt("info", info_path.string());
		warn_ = spdlog::basic_logger_mt("warn", warn_path.string());
	}
	else if (type == "rotating_file") {
		debug_ = spdlog::rotating_logger_mt("debug", debug_path.string(), max_file_size, max_files);
		error_ = spdlog::rotating_logger_mt("error", error_path.string(), max_file_size, max_files);
		info_ = spdlog::rotating_logger_mt("info", info_path.string(), max_file_size, max_files);
		warn_ = spdlog::rotating_logger_mt("warn", warn_path.string(), max_file_size, max_files);
	}
	else if (type == "daily_file") {
		debug_ = spdlog::daily_logger_mt("debug", debug_path.string(), daily_hour, daily_minute);
		error_ = spdlog::daily_logger_mt("error", error_path.string(), daily_hour, daily_minute);
		info_ = spdlog::daily_logger_mt("info", info_path.string(), daily_hour, daily_minute);
		warn_ = spdlog::daily_logger_mt("warn", warn_path.string(), daily_hour, daily_minute);
	}
	else {
		std::println("Invalid logger type: {}", type);
	}

	if (level == "debug") {
		spdlog::set_level(spdlog::level::debug);
	}
	else if (level == "error") {
		spdlog::set_level(spdlog::level::err);
	}
	else if (level == "info") {
		spdlog::set_level(spdlog::level::info);
	}
	else if (level == "warn") {
		spdlog::set_level(spdlog::level::warn);
	}
	//else if (level == "trace") {
	//	spdlog::set_level(spdlog::level::trace);  
	//}
	//else if (level == "critical") {
	//	spdlog::set_level(spdlog::level::critical);
	//}
	else {
		std::println("Unsupport logger level: {}, set level: off", level);
		spdlog::set_level(spdlog::level::off);
	}
}

void logger::debug(const std::string& msg) {
	debug_->debug(msg);
}

void logger::error(const std::string& msg) {
	error_->error(msg);
}

void logger::info(const std::string& msg) {
	info_->info(msg);
}

void logger::warn(const std::string& msg) {
	warn_->warn(msg);
}

std::shared_ptr<spdlog::logger> logger::raw_logger(const std::string& type) {
	return spdlog::get(type);
}

} // namespace leo