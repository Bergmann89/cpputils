#pragma once

#include <utility>
#include <cpputils/mp/core/conditionals.h>
#include <cpputils/mp/misc/tag_of.h>
#include <cpputils/mp/operations/compare/less.h>
#include <cpputils/mp/operations/compare/less_equal.h>
#include <cpputils/mp/operations/compare/greater.h>
#include <cpputils/mp/operations/compare/greater_equal.h>

namespace utl {
namespace mp {

    namespace intern {

        template<typename T>
        struct operators_orderable
            { static constexpr bool value = false; };

    }

    namespace operators
    {
        template<typename X, typename Y, typename = enable_if_c<
            intern::operators_orderable<tag_of<X>>::value &&
            intern::operators_orderable<tag_of<Y>>::value>>
        constexpr auto operator < (X&& x, Y&& y)
            { return less(std::forward<X>(x), std::forward<Y>(y)); }

        template<typename X, typename Y, typename = enable_if_c<
            intern::operators_orderable<tag_of<X>>::value &&
            intern::operators_orderable<tag_of<Y>>::value>>
        constexpr auto operator <= (X&& x, Y&& y)
            { return less_equal(std::forward<X>(x), std::forward<Y>(y)); }

        template<typename X, typename Y, typename = enable_if_c<
            intern::operators_orderable<tag_of<X>>::value &&
            intern::operators_orderable<tag_of<Y>>::value>>
        constexpr auto operator > (X&& x, Y&& y)
            { return greater(std::forward<X>(x), std::forward<Y>(y)); }

        template<typename X, typename Y, typename = enable_if_c<
            intern::operators_orderable<tag_of<X>>::value &&
            intern::operators_orderable<tag_of<Y>>::value>>
        constexpr auto operator >= (X&& x, Y&& y)
            { return greater_equal(std::forward<X>(x), std::forward<Y>(y)); }
    }

}
}