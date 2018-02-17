#pragma once

#include <type_traits>

namespace utl {
namespace mp {

    template<typename T>
    using add_pointer = T*;

    template<typename T>
    using add_reference = T&;

    template<typename T>
    using remove_ptr = typename std::remove_pointer<T>::type;

    template<typename T>
    using remove_const = typename std::remove_const<T>::type;

    template<typename T>
    using remove_cv = typename std::remove_cv<T>::type;

    template<typename T>
    using remove_ref = typename std::remove_reference<T>::type;

    template<typename T>
    using clean_type = remove_cv<remove_ptr<remove_ref<T>>>;

    namespace __impl /* forward declaration */
    {
        template<typename T, typename U = remove_ref<T>>
        struct decay_impl;

        template<typename T, typename U, typename = void>
        struct common_type_impl
            { };
    }

    template<class T>
    using decay = __impl::decay_impl<T>;

    template<class T>
    using decay_type = typename __impl::decay_impl<T>::type;

    template<typename T, typename U>
    using common_type = __impl::common_type_impl<T, U>;

    namespace __impl /* implementation */
    {
        /* decay */
        template <typename T, typename U>
        struct decay_impl
            { using type = remove_cv<U>; };

        template <typename T, typename U>
        struct decay_impl<T, U[]>
            { using type = U*; };

        template <typename T, typename U, std::size_t N>
        struct decay_impl<T, U[N]>
            { using type = U*; };

        template <typename T, typename R, typename ...A>
        struct decay_impl<T, R(A...)>
            { using type = R(*)(A...); };

        template <typename T, typename R, typename ...A>
        struct decay_impl<T, R(A..., ...)>
            { using type = R(*)(A..., ...); };

        /* common_type */
        template<typename T, typename U>
        struct common_type_impl<T, U, decltype((void)(true ? std::declval<T>() : std::declval<U>()))>
            { using type = decay<decltype((void)(true ? std::declval<T>() : std::declval<U>()))>; };
    }

}
}