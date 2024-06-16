#ifndef SYSINFO_WIN_IMPL_HPP
#define SYSINFO_WIN_IMPL_HPP

#include <windows.h>
#include <stdint.h>
#include <pdh.h>

class sysinfo_impl {
    MEMORYSTATUSEX mem_info;
public:
    sysinfo_impl() {
        mem_info.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&mem_info);
    }

    // 获取CPU使用率
    double cpu_usage_impl() {
        static PDH_HQUERY cpu_query;
        static PDH_HCOUNTER cpu_total;
        static bool initialized = false;
        if (!initialized) {
            PdhOpenQuery(NULL, NULL, &cpu_query);
            PdhAddEnglishCounter(cpu_query, "\\Processor(_Total)\\% Processor Time", NULL, &cpu_total);
            PdhCollectQueryData(cpu_query);
            initialized = true;
        }

        PDH_FMT_COUNTERVALUE counter_val;
        PdhCollectQueryData(cpu_query);
        PdhGetFormattedCounterValue(cpu_total, PDH_FMT_DOUBLE, NULL, &counter_val);
        return static_cast<double>(counter_val.doubleValue);
    }

    // 获取可用内存
    uint64_t memory_free_impl() {
        return static_cast<uint64_t>(mem_info.ullAvailPhys);
    }

    // 获取总内存
    uint64_t memory_total_impl() {
        return static_cast<uint64_t>(mem_info.ullTotalPhys);
    }
};

#endif // SYSINFO_WIN_IMPL_HPP
