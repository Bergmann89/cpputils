#pragma once

#include <regex>

#include <cpputils/logging/matcher/matcher.h>

namespace utl {
namespace logging {

    struct matcher_regex : public matcher
    {
    private:
        std::regex  _regex;
        bool        _invert;

    public:
        bool match(const logger& logger) const override;
        bool match(const consumer& consumer) const override;
        matcher_regex(const std::string expression, bool invert = false);
    };

}
}