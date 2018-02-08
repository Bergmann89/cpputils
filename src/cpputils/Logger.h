#pragma once

#include <memory>
#include <chrono>
#include <thread>
#include <sstream>

#include "Exception.h"

// () mandatory
// [] optional
// (logger), (LogLevel: Debug|Info|Warn|Error), [Sender], [Message, [Arguments]]
#define logMessage(logger, level, ...) \
    if (logger.isEnabled(logging::Level::level)) \
        logger.makeLogHelper(logging::Level::level, __FILE__, __LINE__, ## __VA_ARGS__ ) = logging::LogMessage()

// () mandatory
// [] optional
// (LogLevel: Debug|Info|Warn|Error), [Sender], [Message, [Arguments]]
#define logGlobalMessage(level, ...) \
    if (logging::isEnabled(logging::Level::level)) \
        logging::makeLogHelper(logging::Level::level, __FILE__, __LINE__, ## __VA_ARGS__ ) = logging::LogMessage()


namespace logging
{

    enum class Level
    {
        Debug = 0,
        Info,
        Warn,
        Error,
    };

    struct Data
    {
        Level                                   level;
        std::chrono::steady_clock::time_point   time;
        void*                                   sender;
        std::thread::id                         thread;
        const char*                             file;
        int                                     line;
        std::string                             name;
        std::string                             message;
    };
    using DataPtrS = std::shared_ptr<Data>;

    struct Logger;

    struct LogMessage
    {
    private:
        std::ostringstream _msg;

    public:
        inline std::string str() const
            { return _msg.str(); }

        template <typename T>
        inline LogMessage& operator <<(const T& value)
        {
            _msg << value;
            return *this;
        }

        inline LogMessage& operator <<(std::ostream& (*callback)(std::ostream&))
        {
            callback(_msg);
            return *this;
        }

        LogMessage() :
            _msg()
            { }

        LogMessage(const std::string& msg) :
            _msg(msg)
            { }

    private:
        LogMessage(const LogMessage&) = delete;
        LogMessage(LogMessage&&) = delete;
    };

    struct LogHelper
    {
    private:
        Logger&     _logger;
        DataPtrS    _data;

    public:
        inline void operator=(const LogMessage& msg)
            { _data->message += msg.str(); }

        LogHelper(Logger& logger, DataPtrS data);
        ~LogHelper();
    };

    struct Logger
    {
    private:
        Logger(const Logger&) = delete;
        Logger(Logger&&) = delete;

    public:
        Logger() { }

        virtual const std::string& name() const;
        virtual bool isEnabled(Level level) const;
        virtual void log(DataPtrS data) const;

        template<class Sender>
        inline LogHelper makeLogHelper(Level level, const char* file, int line, Sender* sender, std::string message)
            { return makeLogHelper<void>(level, file, line, static_cast<void*>(sender), message); }

        template<class Sender>
        inline LogHelper makeLogHelper(Level level, const char* file, int line, Sender* sender)
            { return makeLogHelper<void>(level, file, line, static_cast<void*>(sender), std::string()); }

        inline LogHelper makeLogHelper(Level level, const char* file, int line, std::string message)
            { return makeLogHelper(level, file, line, nullptr, message); }

        inline LogHelper makeLogHelper(Level level, const char* file, int line)
            { return makeLogHelper(level, file, line, nullptr, std::string()); }

        template<class Sender, class... Args>
        inline LogHelper makeLogHelper(Level level, const char* file, int line, Sender* sender, std::string message, Args... args)
        {
            std::unique_ptr<char, decltype(&free)> buff(static_cast<char*>(malloc(0x8000)), &free);
            auto len = snprintf(buff.get(), 10240, message.c_str(), args...);
            if (len < 0)
                throw utl::ErrorException(errno);
            return makeLogHelper<Sender>(level, file, line, sender, std::string(buff.get(), len));
        }

        template<class... Args>
        inline LogHelper makeLogHelper(Level level, const char* file, int line, std::string message, Args... args)
             { return makeLogHelper<void, Args...>(level, file, line, nullptr, message, args...); }

    };

    template<>
    inline LogHelper Logger::makeLogHelper<void>(Level level, const char* file, int line, void* sender, std::string message)
    {
        DataPtrS data(new Data());
        data->level   = level;
        data->time    = std::chrono::steady_clock::now();
        data->thread  = std::this_thread::get_id();
        data->file    = file;
        data->line    = line;
        data->sender  = sender;
        data->name    = name();
        data->message = message;
        return LogHelper(*this, data);
    }

    Logger& getLogger(const std::string& name = "");

    inline bool isEnabled(Level level)
        { return getLogger().isEnabled(level); }

    template <class Sender>
    inline LogHelper makeLogHelper(Level level, const char* file, int line, Sender* sender, std::string message)
        { return getLogger().makeLogHelper<Sender>(level, file, line, sender, message); }

    template <class Sender>
    inline LogHelper makeLogHelper(Level level, const char* file, int line, Sender* sender)
        { return getLogger().makeLogHelper<Sender>(level, file, line, sender, std::string()); }

    inline LogHelper makeLogHelper(Level level, const char* file, int line, std::string message)
        { return getLogger().makeLogHelper<void>(level, file, line, nullptr, message); }

    inline LogHelper makeLogHelper(Level level, const char* file, int line)
        { return getLogger().makeLogHelper<void>(level, file, line, nullptr, std::string()); }

    template <class Sender, class... Args>
    inline LogHelper makeLogHelper(Level level, const char* file, int line, Sender* sender, std::string message, Args... args)
        { return getLogger().makeLogHelper<Sender, Args...>(level, file, line, sender, message, args...); }

    template <class... Args>
    inline LogHelper makeLogHelper(Level level, const char* file, int line, std::string message, Args... args)
        { return getLogger().makeLogHelper<void, Args...>(level, file, line, nullptr, message, args...); }

    void resetLogging();

}
