#pragma once

#include <cpputils/mp/core/const.h>
#include <cpputils/mp/misc/when.h>
#include <cpputils/mp/misc/tag_of.h>
#include <cpputils/mp/misc/default.h>
#include <cpputils/mp/intern/has_value.h>
#include <cpputils/mp/intern/comparable_equal.h>
#include <cpputils/mp/operations/if.h>
#include <cpputils/mp/operations/value.h>

namespace utl {
namespace mp {

    namespace __impl
    {
        struct equal_t
        {
            template<typename L, typename R>
            constexpr auto operator()(const L& l, const R& r) const;
        };
    }

    constexpr __impl::equal_t equal { };

    namespace __impl
    {
        template <typename L, typename R, typename = void>
        struct equal_impl
            : equal_impl<L, R, when<true>>
            { };

        template <typename L, typename R, bool condition>
        struct equal_impl<L, R, when<condition>>
            : default_
        {
            template <typename ...Args>
            static constexpr auto apply(Args&& ...)
                { return c_false; }
        };

        template <typename T, typename U>
        struct equal_impl<T, U, when<intern::comparable_equal<T, U>::value>>
        {
            template <typename X, typename Y>
            static constexpr auto apply(X&& x, Y&& y)
                { return static_cast<X&&>(x) == static_cast<Y&&>(y); }
        };

        template <typename C>
        struct equal_impl<C, C, when<intern::has_value<C>::value>>
        {
            template <typename X, typename Y>
            static constexpr auto apply(const X&, const Y&)
            {
                constexpr auto eq          = equal(value<X>(), value<Y>());
                constexpr bool truth_value = if_  (eq, true, false);
                return c_bool_t<truth_value> { };
            }
        };

        template<typename L, typename R>
        constexpr auto equal_t::operator()(const L& l, const R& r) const
        {
            using l_tag_type = tag_of<L>;
            using r_tag_type = tag_of<R>;
            using equal_type = equal_impl<l_tag_type, r_tag_type>;
            return equal_type::apply(l, r);
        }
    }

}
}