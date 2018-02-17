#pragma once

#include <set>
#include <mutex>

#include <cpputils/logging/matcher/matcher.h>
#include <cpputils/logging/consumer/consumer.h>

namespace utl {
namespace logging {

    struct rule
    {
    private:
        mutable std::mutex  _mutex;
        std::set<consumer*> _consumer;

    public:
        matcher_ptr_u  logger_matcher;
        matcher_ptr_u  consumer_matcher;
        log_level      min_level;
        log_level      max_level;

        inline bool is_enabled(log_level level) const
        {
            return min_level <= level
                && max_level >= level;
        }

        inline void register_consumer(consumer& consumer)
        {
             std::lock_guard<std::mutex> lk(_mutex);
            _consumer.insert(&consumer);
        }

        inline void unregister_consumer(consumer& consumer)
        {
            std::lock_guard<std::mutex> lk(_mutex);
            _consumer.erase(&consumer);
        }

        inline void log(data_ptr_s data)
        {
            std::lock_guard<std::mutex> lk(_mutex);
            if (is_enabled(data->level))
            {
                for (auto& c : _consumer)
                    c->log(data);
            }
        }

        rule(matcher_ptr_u lm, matcher_ptr_u cm, log_level min, log_level max) :
            logger_matcher  (std::move(lm)),
            consumer_matcher(std::move(cm)),
            min_level       (min),
            max_level       (max)
            { }
    };

    using rule_handle  = void*;

}
}