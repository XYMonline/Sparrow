#ifndef SYSINFO_LINUX_IMPL_HPP
#define SYSINFO_LINUX_IMPL_HPP

#include <stdint.h>

class sysinfo_impl {
public:
    double cpu_usage_impl() {
        static_assert(false, "Not implemented on Linux");
        return 0;
    }

    uint64_t memory_usage_impl() {
        static_assert(false, "Not implemented on Linux");
        return 0;
    }

    uint64_t memory_total_impl() {
        static_assert(false, "Not implemented on Linux");
        return 0;
    }
};

#endif // !SYSINFO_LINUX_IMPL_HPP
