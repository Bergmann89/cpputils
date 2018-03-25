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

    template<typename T_counter, typename... X>
    size_t get_unique_id()
    {
        static const auto v = unique_counter<T_counter>::next();
        return v;
    }

}