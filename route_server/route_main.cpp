#include "../tools/load_blancer.hpp"

#include <print>
#include <thread>

struct session {
	int id_;
	int load_ = 0;
	session(int id) : id_(id) {}
	void do_task() {
		std::println("session {} do task", id_);
		++load_;
	}
	int load() const { return load_; }
	void stop() {}
};

using session_ptr = std::shared_ptr<session>;

int main(int argc, char* argv[]) {
	net::io_context ioc;
	leo::load_balancer<session, leo::least_connections> lb{ ioc };
	
	for (int i = 0; i < 100; ++i) {
		lb.add_server(std::to_string(i), std::make_shared<session>(i));
	}

	auto task = [](session_ptr s) {
		s->do_task();
		};

	std::vector<std::thread> threads;
	threads.emplace_back([&lb, task] {
		for (int i = 0; i < 10000; ++i) {
			lb.commit(task);
		}
		});
	for (int i = 0; i < 32; ++i) {
		threads.emplace_back([&ioc] {
			ioc.run();
			});
	}

	ioc.run();

	for (auto& t : threads) {
		t.join();
	}
}