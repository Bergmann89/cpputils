#pragma once

#include <cpputils/mp/core/checker.h>

namespace utl {
namespace mp {

    namespace __impl /* forward declaration */
    {
        template<typename...>
        struct wrong_impl;
    }

    template<bool condition>
    struct when;

    template<typename... T>
    using when_valid = when<is_valid<T...>::value>;

    template<typename... T>
    using wrong = __impl::wrong_impl<T...>;

    namespace __impl /* implementation */
    {
        template<typename...>
        struct wrong_impl :
            public c_false
            { };
   }

}
}