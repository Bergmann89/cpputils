#pragma once

#include <set>
#include <map>
#include <regex>
#include <mutex>
#include <iostream>

#include "Logger.h"

namespace logging
{

    struct Consumer
    {
    private:
        std::string _name;

    public:
        virtual void log(DataPtrS data) = 0;

        inline const std::string& name() const
            { return _name; }

        Consumer(const std::string& n, bool autoRegister);
        ~Consumer();
    };

    struct StreamConsumer : public Consumer
    {
    private:
        mutable std::mutex  _mutex;
        std::ostream*       _stream;
        bool                _ownsStream;

    public:
        void log(DataPtrS data) override;

        StreamConsumer(const std::string& name, std::ostream& stream, bool ownsStream, bool autoRegister) :
            Consumer    (name, autoRegister),
            _stream     (&stream),
            _ownsStream (ownsStream)
            { }

        virtual ~StreamConsumer()
        {
            if (_ownsStream && _stream)
            {
                delete _stream;
                _stream = nullptr;
            }
        }
    };



    struct Matcher
    {
        virtual bool match(const Logger& logger) const
            { return false; }

        virtual bool match(const Consumer& consumer) const
            { return false; }
    };

    struct AllMatcher : public Matcher
    {
        bool match(const Logger& logger) const override;
        bool match(const Consumer& consumer) const override;
    };

    struct RegexMatcher : public Matcher
    {
    private:
        std::regex  _regex;
        bool        _invert;

    public:
        bool match(const Logger& logger) const override;
        bool match(const Consumer& consumer) const override;

        RegexMatcher(const std::string expression, bool invert = false) :
            _regex  (expression),
            _invert (invert)
            { }
    };

    struct DefaultLoggerMatcher : public Matcher
    {
    private:
        Logger& _defaultLogger;

    public:
        using Matcher::match;
        bool match(const Logger& logger) const override;

        DefaultLoggerMatcher() :
            _defaultLogger(getLogger(std::string()))
            { }
    };

    using RuleHandle  = void*;
    using MatcherPtrU = std::unique_ptr<Matcher>;

    struct Rule
    {
    private:
        mutable std::mutex  _mutex;
        std::set<Consumer*> _consumer;

    public:
        MatcherPtrU         loggerMatcher;
        MatcherPtrU         consumerMatcher;
        Level               minLevel;
        Level               maxLevel;

        inline bool isEnabled(Level level) const
        {
            return minLevel <= level
                && maxLevel >= level;
        }

        inline void registerConsumer(Consumer& consumer)
        {
             std::lock_guard<std::mutex> lk(_mutex);
            _consumer.insert(&consumer);
        }

        inline void unregisterConsumer(Consumer& consumer)
        {
            std::lock_guard<std::mutex> lk(_mutex);
            _consumer.erase(&consumer);
        }

        inline void log(DataPtrS data)
        {
            std::lock_guard<std::mutex> lk(_mutex);
            if (isEnabled(data->level))
            {
                for (auto& c : _consumer)
                    c->log(data);
            }
        }

        Rule(MatcherPtrU lm, MatcherPtrU cm, Level min, Level max) :
            loggerMatcher  (std::move(lm)),
            consumerMatcher(std::move(cm)),
            minLevel       (min),
            maxLevel       (max)
            { }
    };


    struct LoggerImpl : public Logger
    {
    private:
        mutable std::mutex  _mutex;
        std::string         _name;
        std::set<Rule*>     _rules;

    public:
        const std::string& name() const override;
        bool isEnabled(Level level) const override;
        void log(DataPtrS data) const override;

        inline void registerRule(Rule& rule)
        {
            std::lock_guard<std::mutex> lk(_mutex);
            _rules.insert(&rule);
        }

        inline void unregisterRule(Rule& rule)
        {
            std::lock_guard<std::mutex> lk(_mutex);
            _rules.erase(&rule);
        }

        LoggerImpl(const std::string& n) :
            _name(n)
            { }
    };

    void registerConsumer  (Consumer& consumer);
    void unregisterConsumer(Consumer& consumer);

    RuleHandle defineRule(MatcherPtrU loggerMatcher, MatcherPtrU consumerMatcher, Level minLevel = Level::Debug, Level maxLevel = Level::Error);
    void       undefineRule(RuleHandle handle);

}