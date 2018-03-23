#include <cpputils/logging/matcher.h>

using namespace ::utl::logging;

/* matcher */

bool matcher::match(const logger& logger) const
    { return false; }

bool matcher::match(const consumer& consumer) const
    { return false; }

/* matcher_all */

bool matcher_all::match(const logger& logger) const
    { return true; }

bool matcher_all::match(const consumer& consumer) const
    { return true; }

/* matcher_regex */

bool matcher_regex::match(const logger& logger) const
    { return !logger.name().empty() && std::regex_match(logger.name(), _regex) != _invert; }

bool matcher_regex::match(const consumer& consumer) const
    { return !consumer.name().empty() && std::regex_match(consumer.name(), _regex) != _invert; }

matcher_regex::matcher_regex(const std::string expression, bool invert) :
    _regex  (expression),
    _invert (invert)
    { }

/* matcher_default */

bool matcher_default::match(const logger& logger) const
    { return &_defaultLogger == &logger; }

matcher_default::matcher_default() :
    _defaultLogger(get_logger(std::string()))
    { }