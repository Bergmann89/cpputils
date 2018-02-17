
#pragma once

#include <utility>
#include <cpputils/mp/util/when.h>
#include <cpputils/mp/util/default.h>

namespace utl {
namespace mp {

    namespace __impl /* forward declaration */
    {
        template<typename Tag>
        struct make_t;
    }

    template<typename Tag>
    constexpr __impl::make_t<Tag> make { };

    namespace __impl /* implementation */
    {
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