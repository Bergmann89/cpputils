#pragma once

#include <cpputils/mp/misc/when.h>
#include <cpputils/mp/misc/tag_of.h>
#include <cpputils/mp/misc/default.h>

namespace utl {
namespace mp {

    namespace __impl
    {
        template<typename T, typename = void>
        struct value_impl;
    }

    template <typename T>
    constexpr auto value()
    {
        using t_clean_type    = clean_type<T>;
        using tag_type        = tag_of<T>;
        using value_impl_type = __impl::value_impl<tag_type>;
        return value_impl_type::template apply<t_clean_type>();
    }

    template <typename T>
    constexpr decltype(auto) value(T const&)
        { return value<T>(); }

    namespace __impl
    {
        template <typename T, typename>
        struct value_impl
            : value_impl<T, when<true>>
            { };

        template <typename T, bool condition>
        struct value_impl<T, when<condition>>
            : default_
        {
            template <typename ...Args>
            static constexpr auto apply(Args&& ...args) = delete;
        };

        template <typename T>
        struct value_impl<tag_integral_constant<T>>
        {
            template <typename C>
            static constexpr auto apply()
                { return C::value; }
        };
    }

}
}