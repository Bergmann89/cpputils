#pragma once

namespace utl {
namespace mp {

    template<typename...>
    using void_t = void;

    template<typename...>
    struct list { };

    template<typename... T>
    struct inherit : T...
        { };

}
}