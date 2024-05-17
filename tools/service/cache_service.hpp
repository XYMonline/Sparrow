#pragma once
#ifndef SPARROW_SERVICE_HPP
#define SPARROW_SERVICE_HPP

#include "../../base/net.hpp"

#include <sw/redis++/redis++.h>

#include <tuple>
#include <vector>

namespace redis = sw::redis;

namespace leo {
class cache_service {
	std::unique_ptr<redis::Pipeline> pipe_;
	std::unique_ptr<redis::Redis> redis_;
	std::mutex mtx_;

public:
	cache_service() = default;
	~cache_service() = default;

	bool init(redis::ConnectionOptions& options);

	bool signup_service(const std::string& table_name, const std::string& host_port);
	bool remove_service(const std::string& table_name, const std::string& host_port);
	bool check_service(const std::string& key); // key -> host:port
	bool keep_alive(const std::string& key, int64_t timeout);
	bool add_key_value(const std::string& key, const std::string& value, int timeout);
	bool remove_key(const std::string& key);
	bool add_mutex(const std::string& key);
	bool remove_mutex(const std::string& key) { return remove_key(key); }
	bool exists(const std::string& key) { return redis_->exists(key); }
	bool add_hash_field(const std::string& key, const std::string& field, const std::string& value);

	int64_t hashtable_size(const std::string& table_name) { return redis_->hlen(table_name); }

	std::tuple<bool, int64_t>       user_login(const std::string& id, const std::string& pwd, int category);
	std::vector<std::string>        get_services(const std::string& table_name);

};
}

#endif // !SPARROW_SERVICE_HPP
