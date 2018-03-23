#pragma once

#include <cpputils/logging/matcher/matcher.h>

namespace utl {
namespace logging {

    struct matcher_all : public matcher
    {
        bool match(const logger& logger) const override;
        bool match(const consumer& consumer) const override;
    };

}
}
