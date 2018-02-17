#pragma once

#include <cpputils/mp/core/const.h>
#include <cpputils/mp/util/when.h>
#include <cpputils/mp/util/tag_of.h>
#include <cpputils/mp/util/default.h>
#include <cpputils/mp/operations/logical/not.h>
#include <cpputils/mp/operations/compare/less.h>

namespace utl {
namespace mp {

    namespace __impl
    {
        struct greater_equal_t
        {
            template<typename L, typename R>
            constexpr auto operator()(const L& l, const R& r) const;
        };
    }

    constexpr __impl::greater_equal_t greater_equal { };

    namespace __impl
    {
        template <typename L, typename R, typename = void>
        struct greater_equal_impl
            : greater_equal_impl<L, R, when<true>>
            { };

        template <typename T, typename U, bool condition>
        struct greater_equal_impl<T, U, when<condition>>
            : default_
        {
            template <typename X, typename Y>
            static constexpr auto apply(X&& x, Y&& y)
                { return not_(less(static_cast<X&&>(x), static_cast<Y&&>(y))); }
        };

        template<typename L, typename R>
        constexpr auto greater_equal_t::operator()(const L& l, const R& r) const
        {
            using l_tag_type = tag_of<L>;
            using r_tag_type = tag_of<R>;
            using greater_equal_impl_type = greater_equal_impl<l_tag_type, r_tag_type>;
            return greater_equal_impl_type::apply(l, r);
        }
    }

}
}