#include "server.hpp"

namespace leo {

connect_config connect_config_init() {
	auto& loader = config_loader::load_config();
    connect_config conf;

	try {
		conf.db_host			= loader["db_host"].get<std::string>();
		conf.db_port			= loader["db_port"].get<uint16_t>();
		conf.db_name			= loader["db_name"].get<std::string>();
		conf.db_user			= loader["db_user"].get<std::string>();
		conf.db_password		= loader["db_password"].get<std::string>();

		conf.cert_path			= loader["cert_path"].get<std::string>();
		conf.key_path			= loader["key_path"].get<std::string>();
		conf.dh_path			= loader["dh_path"].get<std::string>();

		conf.options.host		= loader["cache_host"].get<std::string>();
		conf.options.port		= loader["cache_port"].get<int>();

		auto cache_user = loader["cache_user"].get<std::string>();
		auto cache_password = loader["cache_password"].get<std::string>();
		if (!cache_user.empty() && !cache_password.empty()) {
			conf.options.user = cache_user;
			conf.options.password = cache_password;
		}
	}
	catch (std::exception const& e) {
		log().error("config error: {}", e.what());
	}

	return conf;
}

}
