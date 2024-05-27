#pragma once
#ifndef SPARROW_WRAP_CONTAINER
#define SPARROW_WRAP_CONTAINER

#include <mutex>
#include <unordered_map>

namespace leo {

template <typename K, typename V>
class wrap_map {
	std::unordered_map<K, V> map_;
	mutable std::recursive_mutex mtx_;
public:
	wrap_map() = default;
	wrap_map(const wrap_map&) = delete;
	wrap_map& operator=(const wrap_map&) = delete;

	auto contains(const K& key) const {
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

	auto clear() {
		std::lock_guard lock(mtx_);
		for (auto& [key, session] : map_) {
			if (session)
				session->stop();
		}
		return map_.clear();
	}

	bool empty() {
		std::lock_guard lock(mtx_);
		return map_.empty();
	}

	size_t size() {
		std::lock_guard lock(mtx_);
		return map_.size();
	}

	void for_each(auto&& func) {
		std::lock_guard lock(mtx_);
		for (auto& [key, session] : map_) {
			func(session);
		}
	}
};

}

#endif // !SPARROW_WRAP_CONTAINER
