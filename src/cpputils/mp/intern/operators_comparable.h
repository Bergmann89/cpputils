#pragma once

#include <utility>
#include <cpputils/mp/core/conditionals.h>
#include <cpputils/mp/util/tag_of.h>
#include <cpputils/mp/operations/compare/equal.h>
#include <cpputils/mp/operations/compare/not_equal.h>

namespace utl {
namespace mp {

    namespace intern {

        template<typename T>
        struct operators_comparable
            { static constexpr bool value = false; };

    }

    namespace operators
    {
        template<typename X, typename Y, typename = enable_if_c<
            intern::operators_comparable<tag_of<X>>::value &&
            intern::operators_comparable<tag_of<Y>>::value>>
        constexpr auto operator == (X&& x, Y&& y)
            { return equal(std::forward<X>(x), std::forward<Y>(y)); }

        template<typename X, typename Y, typename = enable_if_c<
            intern::operators_comparable<tag_of<X>>::value &&
            intern::operators_comparable<tag_of<Y>>::value>>
        constexpr auto operator != (X&& x, Y&& y)
            { return not_equal(std::forward<X>(x), std::forward<Y>(y)); }
    }

}
}