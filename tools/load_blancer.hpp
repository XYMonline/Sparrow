#pragma once
#ifndef SPARROW_LOAD_BALANCER_HPP
#define SPARROW_LOAD_BALANCER_HPP

#include "../base/net.hpp"

#include <print>
#include <memory>
#include <functional>
#include <list>
#include <unordered_map>
#include <map>
#include <mutex>

namespace net = boost::asio;

namespace leo {
;
/*
 * Session must have members:
 * - int load() -> return a comparable value of the load of the session
 * - void stop() -> stop the session
 */
template <typename Session>
struct algorithm_interface {
	using session_ptr = std::shared_ptr<Session>;
	using callback = std::function<void(session_ptr)>;

	virtual void add_server(const std::string& key, session_ptr ptr) = 0;
	virtual void remove_server(const std::string& key) = 0;
	virtual void commit(callback task) = 0;
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual size_t size() const = 0;

};

template <typename Session>
class least_connections 
	: public algorithm_interface<Session> 
	, public std::enable_shared_from_this<least_connections<Session>>
{
	using session_ptr = typename algorithm_interface<Session>::session_ptr;
	using callback = typename algorithm_interface<Session>::callback;

private:
	net::io_context::strand strand_;
	std::map<std::string, session_ptr> servers_; 

public:
	least_connections(net::io_context& ioc)
		: strand_{ ioc }
	{
	}

	~least_connections() {
		int cnt = 0;
		for (auto&& [key, session] : servers_) {
			auto load = session->load();
			std::println("server {} load: {}", key, load);
			cnt += load;
		}
		std::println("total load: {}", cnt);
	}

	void add_server(const std::string& key, session_ptr ptr) override {
		auto self = this->shared_from_this();
		strand_.post([this, key, ptr, self] {
			servers_.emplace(key, ptr);
			});
	}

	void remove_server(const std::string& key) override {
		auto self = this->shared_from_this();
		strand_.post([this, key, self] {
			servers_.erase(key);
			});
	}

	void commit(callback task) override {
		auto self = this->shared_from_this();
		strand_.post([this, task, self] {
			if (servers_.empty()) {
				task(nullptr);
				return;
			}

			auto it = std::min_element(servers_.begin(), servers_.end(),
				[](const auto& a, const auto& b) {
					return a.second->load() < b.second->load();
				});

			if (it != servers_.end()) {
				task(it->second);
			}
			else {
				task(servers_.begin()->second);
			}
			});
	}

	void start() override {
		// Implement start logic if needed
	}

	void stop() override {
		auto self = this->shared_from_this();
		strand_.post([this, self] {
			for (auto&& [key, session] : servers_) {
				session->stop();
			}
			});
	}

	size_t size() const override {
		return servers_.size();
	}
};

template <typename Session, template<typename> class Algorithm>
class load_balancer
	: public algorithm_interface<Session>
{
	using session_ptr = typename algorithm_interface<Session>::session_ptr;
	using callback = typename algorithm_interface<Session>::callback;

	std::shared_ptr<Algorithm<Session>> algorithm_;

public:
	load_balancer(net::io_context& ioc)
		: algorithm_{ std::make_shared<Algorithm<Session>>(ioc) } {
	}

	void add_server(const std::string& key, session_ptr ptr) override {
		algorithm_->add_server(key, ptr);
	}

	void remove_server(const std::string& key) override {
		algorithm_->remove_server(key);
	}

	void commit(callback f) override {
		algorithm_->commit(f);
	}

	void start() override {
		algorithm_->start();
	}

	void stop() override {
		algorithm_->stop();
	}

	size_t size() const override {
		return algorithm_->size();
	}
};

}

#endif // !SPARROW_LOAD_BALANCER_HPP