#pragma once

#include <cxxabi.h>

namespace utl
{

    template<class T>
    struct type_helper
    {
    public:
        static inline std::string name()
        {
            int status;
            auto name = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
            return std::string(name ? name : typeid(T).name());
        }
    };

}