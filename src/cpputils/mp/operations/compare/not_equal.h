#pragma once

#include <cpputils/mp/core/const.h>
#include <cpputils/mp/misc/when.h>
#include <cpputils/mp/misc/tag_of.h>
#include <cpputils/mp/misc/default.h>
#include <cpputils/mp/operations/logical/not.h>
#include <cpputils/mp/operations/compare/not_equal.h>

namespace utl {
namespace mp {

    namespace __impl
    {
        struct not_equal_t
        {
            template<typename L, typename R>
            constexpr auto operator()(const L& l, const R& r) const;
        };
    }

    constexpr __impl::not_equal_t not_equal { };

    namespace __impl
    {
        template <typename L, typename R, typename = void>
        struct not_equal_impl
            : not_equal_impl<L, R, when<true>>
            { };

        template <typename L, typename R, bool condition>
        struct not_equal_impl<L, R, when<condition>>
            : default_
        {
            template <typename X, typename Y>
            static constexpr auto apply(X&& x, Y&& y)
                { return not(equal(std::forward<X>(x), std::forward<Y>(y))); }
        };

        template<typename L, typename R>
        constexpr auto not_equal_t::operator()(const L& l, const R& r) const
        {
            using l_tag_type = tag_of<L>;
            using r_tag_type = tag_of<R>;
            using not_equal_type = not_equal_impl<l_tag_type, r_tag_type>;
            return not_equal_type::apply(l, r);
        }
    }

}
}