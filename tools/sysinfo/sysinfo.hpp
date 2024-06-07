#ifndef SYSINFO_HPP
#define SYSINFO_HPP

#ifdef _WIN32
#include "sysinfo_windows_impl.hpp"
inline sysinfo_impl sysinfo_instance;
#elif __linux__
#include "sysinfo_linux_impl.hpp"
inline sysinfo_impl sysinfo_instance;
#endif

inline double cpu_usage() {
    return sysinfo_instance.cpu_usage_impl();
}

inline uint64_t memory_free() {
    return sysinfo_instance.memory_free_impl();
}

inline uint64_t memory_total() {
    return sysinfo_instance.memory_total_impl();
}

#endif // SYSINFO_HPP
