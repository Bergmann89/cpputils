#include <map>
#include <list>
#include <chrono>
#include <thread>

#include <cpputils/logging/rule.h>
#include <cpputils/logging/global.h>
#include <cpputils/logging/logger_impl.h>
#include <cpputils/logging/consumer/consumer.h>

namespace utl {
namespace logging {

    struct manager
    {
    private:
        logger_impl                                 _default_logger;
        std::map<std::string, logger_impl_ptr_u>    _logger;
        std::list<rule>                             _rules;
        std::set<consumer*>                         _consumer;

        logger_impl& initLogger(logger_impl& logger)
        {
            for (auto& rule : _rules)
            {
                if (rule.logger_matcher->match(logger))
                    logger.registerRule(rule);
            }
            return logger;
        }

    public:
        inline logger& get_logger(const std::string& name)
        {
            if (name.empty())
                return _default_logger;
            auto it = _logger.find(name);
            return (it != _logger.end()
                ? *it->second
                : initLogger(*_logger.emplace(name, logger_impl_ptr_u(new logger_impl(name))).first->second));
        }

        void register_consumer(consumer& consumer)
        {
            _consumer.insert(&consumer);
            for (auto& rule : _rules)
            {
                if (rule.consumer_matcher->match(consumer))
                    rule.register_consumer(consumer);
            }
        }

        void unregister_consumer(consumer& consumer)
        {
            for (auto& rule : _rules)
                rule.unregister_consumer(consumer);
        }

        rule_handle define_rule(matcher_ptr_u logger_matcher, matcher_ptr_u consumer_matcher, log_level min_level, log_level max_level)
        {
            _rules.emplace_back(std::move(logger_matcher), std::move(consumer_matcher), min_level, max_level);
            auto& rule = _rules.back();
            for (auto& c : _consumer)
            {
                if (rule.consumer_matcher->match(*c))
                    rule.register_consumer(*c);
            }
            if (rule.logger_matcher->match(_default_logger))
                _default_logger.registerRule(rule);
            for (auto& l : _logger)
            {
                if (rule.logger_matcher->match(*l.second))
                    l.second->registerRule(rule);
            }
            return &rule;
        }

        void undefine_rule(rule_handle handle)
        {
            auto r = static_cast<rule*>(handle);
            auto it = _rules.begin();
            while (&*it != r && it != _rules.end())
                ++it;
            if (it == _rules.end())
                return;
            _default_logger.unregisterRule(*it);
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

        manager() :
            _default_logger(std::string())
            { }
    };

    manager& get_manager()
    {
        static manager value;
        return value;
    }

    logger& get_logger(const std::string& name)
        { return get_manager().get_logger(name); }

    void register_consumer(consumer& consumer)
        { return get_manager().register_consumer(consumer); }

    void unregister_consumer(consumer& consumer)
        { return get_manager().unregister_consumer(consumer); }

    rule_handle define_rule(matcher_ptr_u logger_matcher, matcher_ptr_u consumer_matcher, log_level min_level, log_level max_level)
        { return get_manager().define_rule(std::move(logger_matcher), std::move(consumer_matcher), min_level, max_level); }

    void undefine_rule(rule_handle rule)
        { return get_manager().undefine_rule(rule); }

    void reset_logging()
        { get_manager().reset(); }

}
}