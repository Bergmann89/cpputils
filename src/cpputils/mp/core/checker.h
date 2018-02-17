#pragma once

#include <type_traits>
#include <cpputils/mp/core/const.h>

namespace utl {
namespace mp {

    namespace __impl /* forward declaration */
    {
        template<typename...>
        struct is_valid_impl;
    }

    template<typename T, typename S>
    using is_same = c_bool<std::is_same<T, S>::value>;

    template<typename Base, typename Derived>
    using is_base_of = c_bool<std::is_base_of<Base, Derived>::value>;

    template<typename... T>
    using is_constructible = c_bool<std::is_constructible<T...>::value>;

    template<typename... T>
    using is_empty = c_bool<std::is_empty<T...>::value>;

    template<typename... T>
    using is_final = c_bool<std::is_final<T...>::value>;

    template<typename T, typename U>
    using is_convertible = c_bool<std::is_convertible<T, U>::value>;

    template<typename T, typename U>
    using is_assignable = c_bool<std::is_assignable<T, U>::value>;

    template<typename... T>
    using is_valid = __impl::is_valid_impl<T...>;

    template<typename T>
    using is_arithmetic = c_bool<std::is_arithmetic<T>::value>;

    namespace __impl /* implementation */
    {
        template<typename...>
        struct is_valid_impl :
            public c_true
            { };
    }

}
}