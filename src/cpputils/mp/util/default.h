#pragma once

#include <cpputils/mp/core/const.h>

namespace utl {
namespace mp {

    namespace __impl /* forward declaration */
    {
        template<typename T, typename = void>
        struct is_default_impl;
    }

    template<typename T>
    using is_default = __impl::is_default_impl<T>;

    namespace __impl /* implementation */
    {
        struct default_ { };

        template<typename T, typename>
        struct is_default_impl :
            public c_false
            { };

        template<typename T>
        struct is_default_impl<T, decltype((void)static_cast<default_>(std::declval<T>()))> :
            public c_true
            { };
   }

}
}