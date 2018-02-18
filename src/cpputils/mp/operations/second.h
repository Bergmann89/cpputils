#pragma once

#include <cpputils/mp/misc/when.h>
#include <cpputils/mp/misc/tag_of.h>
#include <cpputils/mp/misc/default.h>

namespace utl {
namespace mp {

    namespace __impl
    {
        struct second_t
        {
            template<typename T>
            constexpr auto operator()(T&& t) const;
        };
    }

    constexpr __impl::second_t second { };

    namespace __impl
    {
        template <typename T, typename = void>
        struct second_impl
            : second_impl<T, when<true>>
            { };

        template <typename T, bool condition>
        struct second_impl<T, when<condition>>
            : default_
        {
            template <typename ...Args>
            static constexpr auto apply(Args&& ...) = delete;
        };

        template<typename T>
        constexpr auto second_t::operator()(T&& t) const
        {
            using tag   = tag_of<T>;
            using second = second_impl<tag>;
            return second::apply(std::forward<T>(t));
        }
    }

}
}