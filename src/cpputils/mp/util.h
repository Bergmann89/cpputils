#include "core.h"

namespace utl {
namespace mp {

    namespace __impl /* forward declaration */
    {
        /* when */
        template<typename...>
        struct is_valid_impl;

        template<typename...>
        struct wrong_impl;

        /* default */
        template<typename T, typename = void>
        struct is_default_impl;

        /* tag_of */
        template<typename T, typename = void>
        struct tag_of_impl;

        /* is_a */
        template<typename Tag, typename ...T>
        struct is_a_impl;

        /* make */
        template<typename Tag>
        struct make_t;
    }

    /* when */
    template<typename... T>
    using is_valid = __impl::is_valid_impl<T...>;

    template<typename... T>
    using wrong = __impl::wrong_impl<T...>;

    template<bool condition>
    struct when;

    template<typename... T>
    using when_valid = when<is_valid<T...>::value>;

    /* default */
    template<typename T>
    using is_default = __impl::is_default_impl<T>;

    /* tag_of */
    template<typename T>
    using tag_of = typename __impl::tag_of_impl<T>::type;

    /* is_a */
    template<typename Tag, typename... T>
    constexpr __impl::is_a_impl<Tag, T...> is_a { };

    template<typename Tag, typename... T>
    constexpr __impl::is_a_impl<Tag, T...> is_an { };

    /* make */
    template<typename Tag>
    constexpr __impl::make_t<Tag> make { };

    namespace __impl /* implementation */
    {
        /* when */
        template<typename...>
        struct is_valid_impl :
            public c_true
            { };

        template<typename...>
        struct wrong_impl :
            public c_false
            { };

        /* default */
        struct default_ { };

        template<typename T, typename>
        struct is_default_impl :
            public c_false
            { };

        template<typename T>
        struct is_default_impl<T, decltype((void)static_cast<default_>(std::declval<T>()))> :
            public c_true
            { };

        /* tag_of */
        template<typename T, typename>
        struct tag_of_impl :
            public tag_of_impl<T, when<true>>
            { };

        template<typename T, bool condition>
        struct tag_of_impl<T, when<condition>>
            { using type = T; };

        template<typename T>
        struct tag_of_impl<T, when_valid<typename clean_type<T>::tag>>
            { using type = typename clean_type<T>::tag; };

        /* is_a */
        template<typename Tag, typename T>
        struct is_a_impl<Tag, T>  :
            public is_same<Tag, tag_of<T>>
            { };

        template<typename Tag>
        struct is_a_impl<Tag>
        {
            template<typename T>
            constexpr auto operator()(const T&) const noexcept
                { return is_a<Tag, T>; }
        };

        /* make */
        template<typename T, typename = void>
        struct make_impl :
            public make_impl<T, when<true>>
            { };

        template<typename T, bool condition>
        struct make_impl<T, when<condition>> :
            public default_
        {
            template<typename... X>
            static constexpr auto make_helper(int, X&&... x)
                -> decltype(T(std::forward<X>(x)...))
                { return T(std::forward<X>(x)...); }

            template<typename... X>
            static constexpr auto make_helper(long, X&&... x)
                { static_assert((sizeof...(X), false), "there exists no constructor for the given data type"); }

            template <typename... X>
            static constexpr auto apply(X&& ...x)
                { return make_helper(int{}, std::forward<X>(x)...); }
        };

        template<typename T>
        struct make_t
        {
            template<typename... X>
            constexpr auto operator()(X&&... x) const noexcept
                { return make_impl<T>::apply(std::forward<X>(x)...); }
        };
    }
}
}