#pragma once

#include <set>
#include <mutex>

#include <cpputils/logging/rule.h>
#include <cpputils/logging/logger.h>

namespace utl {
namespace logging {

    struct logger_impl : public logger
    {
    private:
        mutable std::mutex  _mutex;
        std::string         _name;
        std::set<rule*>     _rules;

    public:
        const std::string&  name        () const override;
        bool                is_enabled  (log_level level) const override;
        void                log         (data_ptr_s data) const override;

        inline void registerRule(rule& rule)
        {
            std::lock_guard<std::mutex> lk(_mutex);
            _rules.insert(&rule);
        }

        inline void unregisterRule(rule& rule)
        {
            std::lock_guard<std::mutex> lk(_mutex);
            _rules.erase(&rule);
        }

        logger_impl(const std::string& n) :
            _name(n)
            { }
    };

    using logger_impl_ptr_u = std::unique_ptr<logger_impl>;

    void register_consumer  (consumer& consumer);
    void unregister_consumer(consumer& consumer);

    rule_handle define_rule     (matcher_ptr_u logger_matcher, matcher_ptr_u consumer_matcher, log_level min_level = log_level::debug, log_level max_level = log_level::error);
    void        undefine_rule   (rule_handle handle);

    void reset_logging();

} }