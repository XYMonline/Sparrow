#pragma once
#ifndef SPARROW_WRAP_CONTAINER
#define SPARROW_WRAP_CONTAINER

#include <mutex>
#include <unordered_map>

namespace leo {

template <typename K, typename V>
class wrap_map {
	std::unordered_map<K, V> map_;
	std::mutex mtx_;
public:
	wrap_map() = default;
	wrap_map(const wrap_map&) = delete;
	wrap_map& operator=(const wrap_map&) = delete;

	auto contains(auto&& key) {
		std::lock_guard lock(mtx_);
		return map_.contains(key);
	}

	auto emplace(auto&& key, auto&& value) {
		std::lock_guard lock(mtx_);
		return map_.try_emplace(key, value);
	}

	auto erase(auto&& key) {
		std::lock_guard lock(mtx_);
		return map_.erase(key);
	}

	auto find(auto&& key) {
		std::lock_guard lock(mtx_);
		return map_.find(key);
	}

	bool empty() {
		std::lock_guard lock(mtx_);
		return map_.empty();
	}

	size_t size() {
		std::lock_guard lock(mtx_);
		return map_.size();
	}
};

}

#endif // !SPARROW_WRAP_CONTAINER
