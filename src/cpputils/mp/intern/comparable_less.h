#pragma once

#include <utility>
#include <cpputils/mp/core.h>

namespace utl {
namespace mp {
namespace intern {

    template<typename T, typename U = T, typename = void>
    struct comparable_less
        : c_false
        { };

    template<typename T>
    struct comparable_less<T, T, void_t<
        decltype(std::forward<T>(std::declval<T>()) < std::forward<T>(std::declval<T>()) ? 0 : 0)>>
        : c_true
        { };

    template <typename T, typename U>
    struct comparable_less<T, U, enable_if_c<
        !is_same<T, U>::value,
        void_t<
            decltype(std::forward<T>(std::declval<T>()) < std::forward<U>(std::declval<U>()) ? 0 : 0),
            decltype(std::forward<U>(std::declval<U>()) < std::forward<T>(std::declval<T>()) ? 0 : 0),
            common_type<T, U>>>>
        : c_bool<
            comparable_less<T>::value &&
            comparable_less<U>::value &&
            comparable_less<common_type<T, U>>::value>
        { };

}
}
}