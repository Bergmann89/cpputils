#pragma once

#include <chrono>
#include <thread>

namespace utl {
namespace logging {

    enum class log_level
    {
        debug = 0,
        info,
        warn,
        error,
    };

    struct data
    {
        log_level                               level;
        std::chrono::steady_clock::time_point   time;
        const void*                             sender;
        std::thread::id                         thread;
        const char*                             file;
        int                                     line;
        std::string                             name;
        std::string                             message;
    };
    using data_ptr_s = std::shared_ptr<data>;

}
}
