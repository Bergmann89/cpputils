#pragma once

#include <cpputils/mp/core/modifier.h>
#include <cpputils/mp/util/when.h>

namespace utl {
namespace mp {

    namespace __impl /* forward declaration */
    {
        template<typename T, typename = void>
        struct tag_of_impl;
    }

    template<typename T>
    using tag_of = typename __impl::tag_of_impl<clean_type<T>>::type;

    namespace __impl /* implementation */
    {
        template<typename T, typename>
        struct tag_of_impl :
            public tag_of_impl<T, when<true>>
            { };

        template<typename T, bool condition>
        struct tag_of_impl<T, when<condition>>
            { using type = T; };

        template<typename T>
        struct tag_of_impl<T, when_valid<typename clean_type<T>::tag>>
            { using type = typename clean_type<T>::tag; };
   }

}
}