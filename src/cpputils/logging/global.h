#pragma once

#include <cpputils/logging/types.h>
#include <cpputils/logging/logger.h>

// () mandatory
// [] optional
// (LogLevel: debug|info|warn|error), [T_sender], [Message, [Arguments]]
#define log_global_message(level, ...) \
    if (::utl::logging::is_enabled(::utl::logging::log_level::level)) \
        ::utl::logging::make_log_helper(::utl::logging::log_level::level, __FILE__, __LINE__, ## __VA_ARGS__ ) = ::utl::logging::log_message()

namespace utl {
namespace logging {

    inline bool is_enabled(log_level level)
        { return get_logger().is_enabled(level); }

    template <class T_sender>
    inline logger::helper make_log_helper(log_level level, const char* file, int line, T_sender* sender, std::string message)
        { return get_logger().make_log_helper<T_sender>(level, file, line, sender, message); }

    template <class T_sender>
    inline logger::helper make_log_helper(log_level level, const char* file, int line, T_sender* sender)
        { return get_logger().make_log_helper<T_sender>(level, file, line, sender, std::string()); }

    inline logger::helper make_log_helper(log_level level, const char* file, int line, std::string message)
        { return get_logger().make_log_helper<void>(level, file, line, nullptr, message); }

    inline logger::helper make_log_helper(log_level level, const char* file, int line)
        { return get_logger().make_log_helper<void>(level, file, line, nullptr, std::string()); }

    template <class T_sender, class... Args>
    inline logger::helper make_log_helper(log_level level, const char* file, int line, T_sender* sender, std::string message, Args... args)
        { return get_logger().make_log_helper<T_sender, Args...>(level, file, line, sender, message, args...); }

    template <class... Args>
    inline logger::helper make_log_helper(log_level level, const char* file, int line, std::string message, Args... args)
        { return get_logger().make_log_helper<void, Args...>(level, file, line, nullptr, message, args...); }

}
}