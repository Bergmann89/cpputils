#pragma once

#include <cstddef>
#include <cpputils/mp/core/const.h>

namespace utl {
namespace mp {

    template<class TArray>
    struct array_properties;

    template<class T, size_t N>
    struct array_properties<T[N]>
    {
        using size          = c_size_t<N>;
        using value_type    = T;
        using iterator_type = T*;
    };

    template<class T, size_t N>
    struct array_properties<T(&)[N]>
    {
        using size          = c_size_t<N>;
        using value_type    = T;
        using iterator_type = T*;
    };

}
}