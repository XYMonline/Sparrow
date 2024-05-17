#pragma once
#ifndef SPARROW_CONFIG_LOADER_HPP
#define SPARROW_CONFIG_LOADER_HPP

#include <nlohmann/json.hpp>

#include <string_view>
#include <fstream>

using json = nlohmann::json;

namespace leo {
class config_loader {
public:
	static json& load_config(std::string_view path = "../conf/conf.json") {
		static json config;
		if (config.empty()) {
			std::ifstream file{ path.data() };
			file >> config;
		}
		return config;
	};
};
}

#endif // !SPARROW_CONFIG_LOADER_HPP