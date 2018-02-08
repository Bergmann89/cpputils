#include <list>
#include <iomanip>

#include "cpputils/LoggerImpl.h"

using namespace logging;

struct StreamFormatSaver
{
    std::ios        state;
    std::ostream&   stream;

    StreamFormatSaver(std::ostream& s) :
        state   (nullptr),
        stream  (s)
        { state.copyfmt(stream); }

    ~StreamFormatSaver()
        { stream.copyfmt(state); }
};


struct Manager
{
private:
    using LoggerImplPtrU = std::unique_ptr<LoggerImpl>;

    LoggerImpl                              _defaultLogger;
    std::map<std::string, LoggerImplPtrU>   _logger;
    std::list<Rule>                         _rules;
    std::set<Consumer*>                     _consumer;

    LoggerImpl& initLogger(LoggerImpl& logger)
    {
        for (auto& rule : _rules)
        {
            if (rule.loggerMatcher->match(logger))
                logger.registerRule(rule);
        }
        return logger;
    }

public:
    inline Logger& getLogger(const std::string& name)
    {
        if (name.empty())
            return _defaultLogger;
        auto it = _logger.find(name);
        return (it != _logger.end()
            ? *it->second
            : initLogger(*_logger.emplace(name, LoggerImplPtrU(new LoggerImpl(name))).first->second));
    }

    void registerConsumer(Consumer& consumer)
    {
        _consumer.insert(&consumer);
        for (auto& rule : _rules)
        {
            if (rule.consumerMatcher->match(consumer))
                rule.registerConsumer(consumer);
        }
    }

    void unregisterConsumer(Consumer& consumer)
    {
        for (auto& rule : _rules)
            rule.unregisterConsumer(consumer);
    }

    RuleHandle defineRule(MatcherPtrU loggerMatcher, MatcherPtrU consumerMatcher, Level minLevel, Level maxLevel)
    {
        _rules.emplace_back(std::move(loggerMatcher), std::move(consumerMatcher), minLevel, maxLevel);
        auto& rule = _rules.back();
        for (auto& c : _consumer)
        {
            if (rule.consumerMatcher->match(*c))
                rule.registerConsumer(*c);
        }
        if (rule.loggerMatcher->match(_defaultLogger))
            _defaultLogger.registerRule(rule);
        for (auto& l : _logger)
        {
            if (rule.loggerMatcher->match(*l.second))
                l.second->registerRule(rule);
        }
        return &rule;
    }

    void undefineRule(RuleHandle rule)
    {
        auto r = static_cast<Rule*>(rule);
        auto it = _rules.begin();
        while (&*it != r && it != _rules.end())
            ++it;
        if (it == _rules.end())
            return;
        _defaultLogger.unregisterRule(*it);
        for (auto& l : _logger)
            l.second->unregisterRule(*it);
        _rules.erase(it);
    }

    inline void reset()
    {
        _logger.clear();
        _rules.clear();
        _consumer.clear();
    }

    Manager() :
        _defaultLogger(std::string())
        { }
};

Manager& manager()
{
    static Manager value;
    return value;
}

std::string EmptyString;

Consumer::Consumer(const std::string& n, bool autoRegister) :
    _name(n)
{
    if (autoRegister)
        registerConsumer(*this);
}

Consumer::~Consumer()
    { unregisterConsumer(*this); }

void StreamConsumer::log(DataPtrS data)
{
    std::lock_guard<std::mutex> lk(_mutex);

    using namespace std;
    if (!data)
        return;
    auto& d = *data;
    auto t = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(d.time.time_since_epoch()).count();
    auto f = d.file;
    if (f)
    {
        auto tmp = strrchr(f, '/');
        if (tmp)
            f = tmp + 1;
    }
    else
        f = "unknown";

    StreamFormatSaver formatSaver(*_stream);
    if (t >= 0)                         *_stream << "["   << fixed << setfill(' ') << setw(17) <<  setprecision(6) << t << "] ";
    switch(d.level)
    {
        case Level::Debug:              *_stream << "DEBUG "; break;
        case Level::Info:               *_stream << "INFO  "; break;
        case Level::Warn:               *_stream << "WARN  "; break;
        case Level::Error:              *_stream << "ERROR "; break;
    }

    if (d.sender)                       *_stream << "0x"  << hex << setw(2 * sizeof(void*)) << setfill('0') << d.sender;
    else                                *_stream << "                  ";
    if (d.thread != std::thread::id())  *_stream << "@"   << hex << setw(2 * sizeof(void*)) << setfill('0') << d.thread;
    else                                *_stream << "                 ";
    if (!d.name.empty())                *_stream << " '" << d.name << "'";
    if (d.line)                         *_stream << " - " << setw(25) << setfill(' ') << f << ":" << setw(5) << setfill(' ') << dec << d.line;
    if (!d.message.empty())
    {
        *_stream << ": "  << d.message;
        if (d.message.back() != '\n')
            *_stream << std::endl;
    }
    else
        *_stream << std::endl;
}



bool AllMatcher::match(const Logger& logger) const
    { return true; }

bool AllMatcher::match(const Consumer& consumer) const
    { return true; }

bool RegexMatcher::match(const Logger& logger) const
    { return !logger.name().empty() && std::regex_match(logger.name(), _regex) != _invert; }

bool RegexMatcher::match(const Consumer& consumer) const
    { return !consumer.name().empty() && std::regex_match(consumer.name(), _regex) != _invert; }

bool DefaultLoggerMatcher::match(const Logger& logger) const
    { return &_defaultLogger == &logger; }



LogHelper::LogHelper(Logger& logger, DataPtrS data) :
    _logger (logger),
    _data   (data)
    { }

LogHelper::~LogHelper()
    { _logger.log(_data); }



const std::string& Logger::name() const
    { return EmptyString; }

bool Logger::isEnabled(Level level) const
    { return false; }

void Logger::log(DataPtrS data) const
    { /* no op */ }



const std::string& LoggerImpl::name() const
    { return _name; }

bool LoggerImpl::isEnabled(Level level) const
{
    for (auto& rule : _rules)
    {
        if (rule->isEnabled(level))
            return true;
    }
    return false;
}

void LoggerImpl::log(DataPtrS data) const
{
    std::lock_guard<std::mutex> lk(_mutex);
    for (auto& r : _rules)
        r->log(data);
}

namespace logging
{
    Logger& getLogger(const std::string& name)
        { return manager().getLogger(name); }

    void registerConsumer(Consumer& consumer)
        { return manager().registerConsumer(consumer); }

    void unregisterConsumer(Consumer& consumer)
        { return manager().unregisterConsumer(consumer); }

    RuleHandle defineRule(MatcherPtrU loggerMatcher, MatcherPtrU consumerMatcher, Level minLevel, Level maxLevel)
        { return manager().defineRule(std::move(loggerMatcher), std::move(consumerMatcher), minLevel, maxLevel); }

    void undefineRule(RuleHandle rule)
        { return manager().undefineRule(rule); }

    void resetLogging()
        { manager().reset(); }
}