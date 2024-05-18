#include "server.hpp"

namespace leo {

connect_config connect_config_init() {
	auto& loader = config_loader::load_config();
    connect_config conf;

	//////////////////////////////////////
	conf.db_user		= "root";
	conf.db_password	= "1234";
	conf.db_host		= "localhost";
	conf.db_name		= "sparrow";
	/////////////////////////////////////

	conf.options.host			= loader["cache_host"].get<std::string>();
	if (loader.contains("cache_port"))
		conf.options.port		= loader["cache_port"].get<int>();
	if (loader.contains("cache_user"))
		conf.options.user		= loader["cache_user"].get<std::string>();
	if (loader.contains("cache_password"))
		conf.options.password	= loader["cache_password"].get<std::string>();

	if (loader.contains("db_user"))
		conf.db_user			= loader["db_user"].get<std::string>();
	if (loader.contains("db_port"))
		conf.db_port			= loader["db_port"].get<uint16_t>();
	if (loader.contains("db_name"))
		conf.db_name			= loader["db_name"].get<std::string>();

	conf.cert_path				= loader["cert_path"].get<std::string>();
	conf.key_path				=loader["private_key_path"].get<std::string>();
	conf.dh_path				= loader["dh_path"].get<std::string>();

	return conf;
}

}
