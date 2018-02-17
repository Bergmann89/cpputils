#pragma once

#include <cstddef>
#include <type_traits>

namespace utl {
namespace mp {

    template<typename T>
    struct tag_const { };

    template<typename T, T t>
    struct integral_constant :
        public std::integral_constant<T, t>
    {
        using tag = tag_const<T>;
    };

    template<bool B>
    using c_bool = integral_constant<bool, B>;

    template<size_t S>
    using c_size_t = integral_constant<size_t, S>;

    using c_true  = c_bool<true>;
    using c_false = c_bool<false>;
    using c_zero  = c_size_t<0>;

}
}