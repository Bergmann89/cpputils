#pragma once

#include <cpputils/mp/misc/to.h>
#include <cpputils/mp/misc/make.h>
#include <cpputils/mp/container/basic_tuple.h>

namespace utl {
namespace mp {

    struct tag_map { };

    constexpr auto make_map = make<tag_map>;

    constexpr auto to_map = make<tag_map>;

    namespace __impl /* forward declaration */
    {
        template<typename... Pairs>
        struct make_map_type;
    }

    template<typename... Pairs>
    using map = typename __impl::make_map_type<Pairs...>::type;

    namespace __impl /* implementation */
    {
        template<typename... Pairs>
        struct make_map_type
        {
            using storage_type = basic_tuple<Pairs...>;
            
        };
    }

}
}