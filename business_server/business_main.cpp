#include "business_server.hpp"
#include "../tools/config_loader.hpp"

#include <boost/asio/signal_set.hpp>

#include <vector>

int main(int argc, char* argv[]) {
    net::io_context ioc;
    leo::business::business_server server{ ioc };
    server.start();

    auto& cancellation = server.signals();
    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait(
        [&](beast::error_code const&, int sig) {
            if (sig == SIGINT) {
                cancellation.emit(net::cancellation_type::all);
            }
            else {
                ioc.stop();
            }
        });

    auto work_thread_num = std::max(
        4u,
        std::min(
            std::thread::hardware_concurrency(),
            leo::config_loader::load_config()["work_thread_num"].get<uint32_t>()
        )
    );

    std::vector<std::thread> threads;
    for (auto i = 1u; i < work_thread_num; ++i) {
        threads.emplace_back([&ioc] { ioc.run(); });
    }
    ioc.run();

    for (auto& t : threads) {
        t.join();
    }

    // after all threads are joined, store the cache
    server.store();
}
