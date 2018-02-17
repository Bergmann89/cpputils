#pragma once

#include <cpputils/mp/core/const.h>
#include <cpputils/mp/util/when.h>
#include <cpputils/mp/util/tag_of.h>
#include <cpputils/mp/util/default.h>

namespace utl {
namespace mp {

    namespace __impl
    {
        struct not_t
        {
            template<typename X>
            constexpr auto operator()(X&& x) const;
        };
    }

    constexpr __impl::not_t not_ { };

    namespace __impl
    {
        template <typename T, typename = void>
        struct not_impl
            : not_impl<T, when<true>>
            { };

        template <typename T, bool condition>
        struct not_impl<T, when<condition>>
            : default_
        {
            template <typename ...Args>
            static constexpr auto apply(Args&& ...) = delete;
        };

        template <typename T>
        struct not_impl<T, when<is_arithmetic<T>::value>>
        {
            template <typename X>
            static constexpr auto apply(const X& x)
                { return x ? false : true; }
        };

        template <typename X>
        constexpr auto not_t::operator()(X&& x) const
        {
            using tag_type = tag_of<X>;
            using not_impl_type = not_impl<tag_type>;
            return not_impl_type::apply(std::forward<X>(x));
        };
    }

}
}