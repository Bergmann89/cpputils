#pragma once

#include <memory>

#include <cpputils/logging/logger.h>
#include <cpputils/logging/consumer/consumer.h>

namespace utl {
namespace logging {

    struct matcher
    {
        virtual bool match(const logger& logger) const;
        virtual bool match(const consumer& consumer) const;
    };

    using matcher_ptr_u = std::unique_ptr<matcher>;

}
}
