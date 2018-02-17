#pragma once

#include <cpputils/mp/operations/value.h>

namespace utl {
namespace mp {
namespace intern {

    template<typename C>
    struct has_value
        : c_bool<!is_default<__impl::value_impl<tag_of<C>>>::value>
        { };

}
}
}