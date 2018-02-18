#pragma once

#include <cstddef>
#include <type_traits>

namespace utl {
namespace mp {

    template<typename T>
    struct tag_integral_constant { };

    template<typename T, T t>
    struct integral_constant :
        public std::integral_constant<T, t>
    {
        using tag = tag_integral_constant<T>;
    };

    template<bool B>
    using c_bool_t = integral_constant<bool, B>;

    template<size_t S>
    using c_size_t = integral_constant<size_t, S>;

    using c_zero_t  = c_size_t<0>;
    using c_true_t  = c_bool_t<true>;
    using c_false_t = c_bool_t<false>;

    template<size_t S>
    constexpr c_size_t<S> c_size { };

    constexpr c_zero_t  c_zero { };
    constexpr c_true_t  c_true { };
    constexpr c_false_t c_false { };

}
}