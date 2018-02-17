#pragma once

#include <cpputils/logging/matcher/matcher.h>

namespace utl {
namespace logging {

    struct matcher_default : public matcher
    {
    private:
        logger& _defaultLogger;

    public:
        using matcher::match;
        bool match(const logger& logger) const override;
        matcher_default();
    };

}
}