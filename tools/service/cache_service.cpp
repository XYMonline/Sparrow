#include "cache_service.hpp"

#include <chrono>
#include <print>

namespace leo {
;
bool cache_service::init(redis::ConnectionOptions& options) {
	try {
		redis_ = std::make_unique<redis::Redis>(options);
		pipe_ = std::make_unique<redis::Pipeline>(std::move(redis_->pipeline(false)));
	}
	catch (const std::exception& e) {
		std::println("error: cache_service::init: {}", e.what());
		return false;
	}
	return true;
}

bool cache_service::signup_service(const std::string& table_name, const std::string& host_port) {
	return redis_->sadd(table_name.data(), host_port);
}

bool cache_service::remove_service(const std::string& table_name, const std::string& host_port) {
	std::lock_guard lock(mtx_);
	auto replies = pipe_->del(host_port).srem(table_name, host_port).exec();
	return replies.get<bool>(0) && replies.get<bool>(1);
}

bool cache_service::check_service(const std::string& key) {
	return redis_->exists(key);
}

bool cache_service::keep_alive(const std::string& key, int64_t timeout) {
	return redis_->set(key, ".", std::chrono::seconds(timeout));
}

bool cache_service::add_key_value(const std::string& key, const std::string& value, int timeout) {
	return redis_->set(key, value, std::chrono::seconds(timeout));
}

bool cache_service::remove_key(const std::string& key) {
	return redis_->del(key);
}

bool cache_service::add_mutex(const std::string& key) {
	std::lock_guard lock(mtx_);
	auto replies = pipe_->setnx(key, ".").expire(key, 30).exec(); // 30秒内持有锁
	return replies.get<bool>(0) && replies.get<bool>(1);
}

bool cache_service::add_hash_field(const std::string& key, const std::string& field, const std::string& value) {
	return redis_->hset(key, field, value);
}

std::tuple<bool, int64_t> cache_service::user_login(const std::string& id, const std::string& pwd, int category) {
	redis::OptionalString res;
	//if (category == message_type::REQUEST_SIGN_BACK) {
	//      res = redis_->get(id);
	//      if (res) {
	//              if (res.value() == pwd) {
	//                      return { true, std::stoll(id)}; // id is formal_id
	//              }
	//              return { false, 1 }; // pwd error
	//      }
	//}
	//else {
	//      res = redis_->hget("user_main", id);
	//      if (res) {
	//              auto j = json::parse(res.value());
	//              if (j[1] == pwd) {
	//                      return { true, j[0]};
	//              }
	//              return { false, 1 }; // pwd error
	//      }
	//}
	return { false, 0 }; // not found
}


std::vector<std::string> cache_service::get_services(const std::string& table_name) {
	std::vector<std::string> services;
	redis_->smembers(table_name, std::back_inserter(services));
	return services;
}

}
