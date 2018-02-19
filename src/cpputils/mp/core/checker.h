#pragma once

#include <type_traits>
#include <cpputils/mp/core/const.h>

namespace utl {
namespace mp {

    namespace __impl /* forward declaration */
    {
        template<typename...>
        struct is_valid_impl;

        template<bool...>
        struct bool_pack;

        template<typename T, template <typename...> class T_template>
        struct is_specialization_of_impl
            : c_false_t
            { };

        template<template <typename...> class T_template, typename... Ts>
        struct is_specialization_of_impl<T_template<Ts...>, T_template>
            : c_true_t
            { };
    }

    template<typename T, typename S>
    using is_same = c_bool_t<std::is_same<T, S>::value>;

    template<typename Base, typename Derived>
    using is_base_of = c_bool_t<std::is_base_of<Base, Derived>::value>;

    template<typename... T>
    using is_constructible = c_bool_t<std::is_constructible<T...>::value>;

    template<typename... T>
    using is_empty = c_bool_t<std::is_empty<T...>::value>;

    template<typename... T>
    using is_final = c_bool_t<std::is_final<T...>::value>;

    template<typename T, typename U>
    using is_convertible = c_bool_t<std::is_convertible<T, U>::value>;

    template<typename T, typename U>
    using is_assignable = c_bool_t<std::is_assignable<T, U>::value>;

    template<typename... T>
    using is_valid = __impl::is_valid_impl<T...>;

    template<typename T>
    using is_arithmetic = c_bool_t<std::is_arithmetic<T>::value>;

    template<typename T>
    using is_signed = c_bool_t<std::is_signed<T>::value>;

    template<typename T>
    using is_unsigned = c_bool_t<std::is_unsigned<T>::value>;

    template<typename T, template <typename...> class T_template>
    using is_specialization_of = __impl::is_specialization_of_impl<T, T_template>;

    template<bool... B>
    using all_true = c_bool_t<std::is_same<__impl::bool_pack<true, B...>, __impl::bool_pack<B..., true>>::value>;

    namespace __impl /* implementation */
    {
        template<typename...>
        struct is_valid_impl :
            public c_true_t
            { };
    }

}
}