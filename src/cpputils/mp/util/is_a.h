#pragma once

#include <cpputils/mp/util/when.h>
#include <cpputils/mp/util/tag_of.h>

namespace utl {
namespace mp {

    namespace __impl /* forward declaration */
    {
        template<typename Tag, typename ...T>
        struct is_a_impl;
    }

    template<typename Tag, typename... T>
    constexpr __impl::is_a_impl<Tag, T...> is_a { };

    template<typename Tag, typename... T>
    constexpr __impl::is_a_impl<Tag, T...> is_an { };

    namespace __impl /* implementation */
    {
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
   }

}
}