#pragma once

namespace utl {
namespace mp {

    namespace __impl /* forward declaration */
    {
        template<bool C, typename T, typename E>
        struct if_c_impl;

        template<bool C, typename T, template<typename...> class E, typename... A>
        struct eval_if_c_impl;
    }

    template<typename T, typename S = void>
    using enable_if = typename std::enable_if<T::value, S>::type;

    template<bool B, typename S = void>
    using enable_if_c = typename std::enable_if<B, S>::type;

    template<typename B, typename T, typename E>
    using if_t = typename __impl::if_c_impl<B::value != 0, T, E>::type;

    template<bool B, typename T, typename E>
    using if_c = typename __impl::if_c_impl<B, T, E>::type;

    template<typename C, typename T, template<typename...> class E, typename... A>
    using eval_if_t = typename __impl::eval_if_c_impl<C::value != 0, T, E, A...>::type;

    template<bool C, typename T, template<typename...> class E, typename... A>
    using eval_if_c = typename __impl::eval_if_c_impl<C, T, E, A...>::type;

    namespace __impl /* implementation */
    {
        template<typename T, typename F>
        struct if_c_impl<true, T, F>
            { using type = T; };

        template<typename T, typename F>
        struct if_c_impl<false, T, F>
            { using type = F; };

        template<typename T, template<typename...> class E, typename... A>
        struct eval_if_c_impl<true, T, E, A...>
            { using type = T; };

        template<typename T, template<typename...> class E, typename... A>
        struct eval_if_c_impl<false, T, E, A...>
            { using type = E<A...>; };
    }

}
}