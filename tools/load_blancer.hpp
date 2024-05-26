#pragma once
#ifndef SPARROW_LOAD_BALANCER_HPP
#define SPARROW_LOAD_BALANCER_HPP

#include <boost/asio.hpp>

#include <print>
#include <memory>
#include <functional>
#include <list>
#include <unordered_map>
#include <map>
#include <mutex>
#include <atomic>

namespace net = boost::asio;

namespace leo {
;
/*
	 * Session must have members:
	 * - int load() -> return a comparable value of the load of the session
	 * - void stop() -> stop the session
	 */
template <typename Session, template<typename> class Derived>
struct algorithm_interface {
	using session_ptr = std::shared_ptr<Session>;
	using callback = std::function<void(session_ptr)>;

	void add_server(const std::string& key, session_ptr ptr) {
		derived().add_server_impl(key, ptr);
	}

	void remove_server(const std::string& key) {
		derived().remove_server_impl(key);
	}

	void commit(callback task) {
		derived().commit_impl(task);
	}

	void start() {
		derived().start_impl();
	}

	void stop() {
		derived().stop_impl();
	}

	size_t size() const {
		return derived().size_impl();
	}

private:
	Derived<Session>& derived() {
		return static_cast<Derived<Session>&>(*this);
	}
};

template <typename Session>
class least_connections
	: public algorithm_interface<Session, least_connections>
	, public std::enable_shared_from_this<least_connections<Session>>
{

public:
	using session_ptr = typename algorithm_interface<Session, least_connections>::session_ptr;
	using callback = typename algorithm_interface<Session, least_connections>::callback;

private:
	net::io_context::strand strand_;
	std::map<std::string, session_ptr> servers_;

public:
	least_connections(net::io_context& ioc)
		: strand_{ ioc } {
	}

	~least_connections() = default;

	void add_server_impl(const std::string& key, session_ptr ptr) {
		auto self = this->shared_from_this();
		strand_.post([this, key, ptr, self] {
			servers_.emplace(key, ptr);
			});
	}

	void remove_server_impl(const std::string& key) {
		auto self = this->shared_from_this();
		strand_.post([this, key, self] {
			servers_.erase(key);
			});
	}

	void commit_impl(callback task) {
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

	void start_impl() {
		// Implement start logic if needed
	}

	void stop_impl() {
		auto self = this->shared_from_this();
		strand_.post([this, self] {
			for (auto&& [key, session] : servers_) {
				session->stop();
			}
			servers_.clear();
			});
	}

	size_t size_impl() const {
		return servers_.size();
	}
};

template <typename Session, template<typename> class Algorithm>
class load_balancer {
	using session_ptr = typename Algorithm<Session>::session_ptr;
	using callback = typename Algorithm<Session>::callback;

	std::shared_ptr<Algorithm<Session>> algorithm_;

public:
	load_balancer(net::io_context& ioc)
		: algorithm_{ std::make_shared<Algorithm<Session>>(ioc) } {
	}

	void add_server(const std::string& key, session_ptr ptr) {
		algorithm_->add_server(key, ptr);
	}

	void remove_server(const std::string& key) {
		algorithm_->remove_server(key);
	}

	void commit(callback f) {
		algorithm_->commit(f);
	}

	void start() {
		algorithm_->start();
	}

	void stop() {
		algorithm_->stop();
	}

	size_t size() const {
		return algorithm_->size();
	}
};

}

#endif // !SPARROW_LOAD_BALANCER_HPP

#ifdef LEO_LOAD_BALANCER_TEST
struct session : public std::enable_shared_from_this<session> {
	std::atomic<int> load_ = 0;
	int id_;
	session(int id) : id_{ id } {}
	void stop() {}
	void do_task() {
		std::println("server{} task{}", id_, load_.load());
		++load_;
	}
	int load() {
		return load_;
	}
};

#include <random>
#include <thread>
boost::asio::io_context ioc;
std::random_device rd;

int main() {
	leo::load_balancer<session, leo::least_connections> lb(ioc);

	for (int i = 0; i < 100; ++i) {
		lb.add_server(std::to_string(i), std::make_shared<session>(i));
	}

	std::vector<std::thread> threads;
	for (int i = 0; i < 100; ++i) {
		threads.emplace_back([&] {
			for (int j = 0; j < 100; ++j) {
				lb.commit([i, j](auto&& session) {
					if (session) {
						session->do_task();
					}
					});
			}
			});
	}

	ioc.run();

	for (auto& t : threads) {
		t.join();
	}
}
#endif // LEO_LOAD_BALANCER_TEST