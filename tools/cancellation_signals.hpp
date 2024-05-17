#pragma once
#ifndef SPARROW_CANCELLATION_SIGNALS_HPP
#define SPARROW_CANCELLATION_SIGNALS_HPP

#include "../base/net.hpp"

#include <list>
#include <mutex>

namespace leo {
;
struct cancellation_signals {
    std::list<net::cancellation_signal> sigs;
    std::mutex mtx;
    void emit(net::cancellation_type ct = net::cancellation_type::all);

    net::cancellation_slot slot();
};
}

#endif // !SPARROW_CANCELLATION_SIGNALS_HPP
