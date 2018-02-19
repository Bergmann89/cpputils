#pragma once

namespace utl {
namespace mp {

    namespace __impl
    {
        template <typename ...>
        struct always_true
            { static constexpr bool value = true; };
    }

    template <bool condition>
    struct when;

    template <typename... T>
    using when_valid = when<__impl::always_true<T...>::value>;

}
}