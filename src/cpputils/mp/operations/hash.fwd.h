#pragma once

#include <cpputils/mp/misc/when.h>
#include <cpputils/mp/misc/tag_of.h>
#include <cpputils/mp/misc/default.h>
#include <cpputils/mp/core/const.h>
#include <cpputils/mp/core/modifier.h>
#include <cpputils/mp/container/type.h>

namespace utl {
namespace mp {

    namespace __impl /* forward declaration */
    {
        struct hash_t
        {
            template<typename X>
            constexpr auto operator()(const X& x) const;
        };

        template <typename T, typename = void>
        struct hash_impl;
    }

    constexpr __impl::hash_t hash { };

}
}