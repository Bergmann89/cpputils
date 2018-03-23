#pragma once

#include <cpputils/misc/exception.h>
#include <cpputils/logging/types.h>

// () mandatory
// [] optional
// (logger), (LogLevel: debug|info|warn|error), [Sender], [Message, [Arguments]]
#define log_message(logger, level, ...) \
    if (logger.is_enabled(::utl::logging::log_level::level)) \
        logger.make_log_helper(::utl::logging::log_level::level, __FILE__, __LINE__, ## __VA_ARGS__ ) = std::ostringstream()

namespace utl {
namespace logging {

    struct logger
    {
    public:
        struct helper
        {
        private:
            logger&     _logger;
            data_ptr_s  _data;

        public:
            inline void operator=(const std::ostream& os)
            {
                auto* msg = dynamic_cast<const std::ostringstream*>(&os);
                if (msg)
                    _data->message += msg->str();
            }

            inline helper(logger& logger, data_ptr_s data) :
                _logger (logger),
                _data   (data)
                { }

            inline ~helper()
                { _logger.log(_data); }

        public:
            static inline helper create(logger& logger, log_level level, const char* file, int line, void* sender, const std::string& name, std::string message)
            {
                using namespace ::utl::logging;
                data_ptr_s ret(new data());
                ret->level   = level;
                ret->time    = std::chrono::steady_clock::now();
                ret->thread  = std::this_thread::get_id();
                ret->file    = file;
                ret->line    = line;
                ret->sender  = sender;
                ret->name    = name;
                ret->message = message;
                return helper(logger, ret);
            }
        };

    private:
        logger(logger&&) = delete;
        logger(const logger&) = delete;

    public:
        logger() { }

        virtual const std::string&  name        () const;
        virtual bool                is_enabled  (log_level level) const;
        virtual void                log         (data_ptr_s data) const;

        template<class Sender, class... Args>
        inline logger::helper make_log_helper(log_level level, const char* file, int line, Sender* sender, std::string message, Args... args)
        {
            std::unique_ptr<char, decltype(&free)> buff(static_cast<char*>(malloc(0x8000)), &free);
            auto len = snprintf(buff.get(), 10240, message.c_str(), args...);
            if (len < 0)
                throw utl::error_exception(errno);
            return helper::create(*this, level, file, line, static_cast<void*>(sender), name(), std::string(buff.get(), len));
        }

        template<class... Args>
        inline logger::helper make_log_helper(log_level level, const char* file, int line, std::string message, Args... args)
             { return make_log_helper<void, Args...>(level, file, line, nullptr, message, args...); }

        template<class Sender>
        inline logger::helper make_log_helper(log_level level, const char* file, int line, Sender* sender, std::string message)
            { return helper::create(*this, level, file, line, static_cast<void*>(sender), name(), message); }

        template<class Sender>
        inline logger::helper make_log_helper(log_level level, const char* file, int line, Sender* sender)
            { return make_log_helper<void>(level, file, line, static_cast<void*>(sender), std::string()); }

        inline logger::helper make_log_helper(log_level level, const char* file, int line, std::string message)
            { return make_log_helper<void>(level, file, line, nullptr, message); }

        inline logger::helper make_log_helper(log_level level, const char* file, int line)
            { return make_log_helper<void>(level, file, line, nullptr, std::string()); }
    };

    logger& get_logger(const std::string& name = "");

}
}
