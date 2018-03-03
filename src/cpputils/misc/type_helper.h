#pragma once

#include <cxxabi.h>

namespace utl
{

    template<typename T>
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

    template<typename...>
    struct unique_counter
    {
        static size_t& next()
        {
            static size_t value { };
            return ++value;
        }
    };

    template<typename... X>
    struct unique_id
        {   static const size_t value = unique_counter<X...>::next(); };

}