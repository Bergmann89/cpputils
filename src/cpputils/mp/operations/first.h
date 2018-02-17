#pragma once

#include <cpputils/mp/util/when.h>
#include <cpputils/mp/util/tag_of.h>
#include <cpputils/mp/util/default.h>

namespace utl {
namespace mp {

    namespace __impl
    {
        struct first_t
        {
            template<typename T>
            constexpr auto operator()(T&& t) const;
        };
    }

    constexpr __impl::first_t first { };

    namespace __impl
    {
        template <typename T, typename = void>
        struct first_impl
            : first_impl<T, when<true>>
            { };

        template <typename T, bool condition>
        struct first_impl<T, when<condition>>
            : default_
        {
            template <typename ...Args>
            static constexpr auto apply(Args&& ...) = delete;
        };

        template<typename T>
        constexpr auto first_t::operator()(T&& t) const
        {
            using tag_type   = tag_of<T>;
            using first_type = first_impl<tag_type>;
            return first_type::apply(std::forward<T>(t));
        }
    }

}
}