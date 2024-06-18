#include "logger.hpp"
#include "../config_loader.hpp"

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

	auto log_path = fs::path{ path };

	if (type == "console") {
		log_ = spdlog::stdout_color_mt("console");
	}
	else if (type == "basic") {
		log_ = spdlog::basic_logger_mt("basic_file", log_path.string() + "log.txt");
	}
	else if (type == "rotating") {
		log_ = spdlog::rotating_logger_mt("rotating_file", log_path.string() + "rotateing.txt", max_file_size, max_files);
	}
	else if (type == "daily") {
		log_ = spdlog::daily_logger_mt("daily_file", log_path.string() + "daily.txt", daily_hour, daily_minute);
	}
	else {
		raw_logger("console")->error("Invalid logger type: {}, set type console", type);
		log_ = spdlog::stdout_color_mt("console");
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
		raw_logger("console")->error("Unsupport logger level: {}, set level: off", level);
		spdlog::set_level(spdlog::level::off);
	}
}

std::shared_ptr<spdlog::logger> logger::raw_logger(const std::string& type) {
	return spdlog::get(type);
}

logger& log() {
	static bool init{ false };
	static auto& conf = config_loader::load_config();
	static logger log_{
		conf.contains("log_type") ? conf["log_type"].get<std::string>() : "console",
		conf.contains("log_level") ? conf["log_level"].get<std::string>() : "debug",
		conf.contains("log_path") ? conf["log_path"].get<std::string>() : "../logs",
		conf.contains("max_file_size") ? conf["max_file_size"].get<size_t>() : 104857600,
		conf.contains("max_files") ? conf["max_files"].get<int>() : 3,
		conf.contains("daily_hour") ? conf["daily_hour"].get<int>() : 0,
		conf.contains("daily_minute") ? conf["daily_minute"].get<int>() : 0,
	};

	return log_;
}

} // namespace leo