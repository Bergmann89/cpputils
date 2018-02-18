#pragma once

#include <utility>
#include <cpputils/mp/core.h>

namespace utl {
namespace mp {
namespace intern {

    template<typename T, typename U = T, typename = void>
    struct comparable_equal
        : c_false_t
        { };

    template<typename T>
    struct comparable_equal<T, T, void_t<
        decltype(std::forward<T>(std::declval<T>()) == std::forward<T>(std::declval<T>()) ? 0 : 0),
        decltype(std::forward<T>(std::declval<T>()) != std::forward<T>(std::declval<T>()) ? 0 : 0)>>
        : c_true_t
        { };

    template <typename T, typename U>
    struct comparable_equal<T, U, enable_if_c<
        !is_same<T, U>::value,
        void_t<
            decltype(std::forward<T>(std::declval<T>()) == std::forward<U>(std::declval<U>()) ? 0 : 0),
            decltype(std::forward<U>(std::declval<U>()) == std::forward<T>(std::declval<T>()) ? 0 : 0),
            decltype(std::forward<T>(std::declval<T>()) != std::forward<U>(std::declval<U>()) ? 0 : 0),
            decltype(std::forward<U>(std::declval<U>()) != std::forward<T>(std::declval<T>()) ? 0 : 0),
            common_type<T, U>>>>
        : c_bool_t<
            comparable_equal<T>::value &&
            comparable_equal<U>::value &&
            comparable_equal<common_type<T, U>>::value>
        { };

}
}
}