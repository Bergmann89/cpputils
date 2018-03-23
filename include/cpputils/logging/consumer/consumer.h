#pragma once

#include <cpputils/logging/types.h>

namespace utl {
namespace logging {

    struct consumer
    {
    private:
        std::string _name;

    public:
        virtual void log(data_ptr_s data) = 0;

        inline const std::string& name() const
            { return _name; }

        consumer(const std::string& n, bool autoRegister);
        ~consumer();
    };

}
}
