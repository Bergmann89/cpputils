#pragma once

#include <cpputils/mp/core/const.h>
#include <cpputils/mp/util/when.h>
#include <cpputils/mp/util/tag_of.h>
#include <cpputils/mp/util/default.h>
#include <cpputils/mp/operations/if.h>

namespace utl {
namespace mp {

    namespace __impl
    {
        struct and_t
        {
            template<typename X, typename Y>
            constexpr auto operator()(X&& x, Y&& y) const;

            template<typename X, typename... Y>
            constexpr auto operator()(X&& x, Y&&... y) const;
        };
    }

    constexpr __impl::and_t and_ { };

    namespace __impl
    {
        template <typename T, typename = void>
        struct and_impl
            : and_impl<T, when<true>>
            { };

        template <typename T, bool condition>
        struct and_impl<T, when<condition>>
            : default_
        {
            template <typename X, typename Y>
            static constexpr auto apply(X&& x, Y&& y)
                { return if_(x, static_cast<Y&&>(y), x); }
        };

        template <typename X, typename Y>
        constexpr auto and_t::operator()(X&& x, Y&& y) const
        {
            using tag_type = tag_of<X>;
            using and_impl_type = and_impl<tag_type>;
            return and_impl_type::apply(std::forward<X>(x), std::forward<Y>(y));
        };
        /* TODO
        template <typename X, typename ...Y>
        constexpr auto and_t::operator()(X&& x, Y&& ...y) const
        {
            return detail::variadic::foldl1(
                *this,
                static_cast<X&&>(x),
                static_cast<Y&&>(y)...
            );
        }
        */
    }

}
}