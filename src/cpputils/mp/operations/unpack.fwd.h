#pragma once

namespace utl {
namespace mp {

    namespace __impl
    {
        struct unpack_t
        {
            template<typename Xs, typename F>
            constexpr auto operator()(Xs&& xs, F&& f) const;
        };
    }

    constexpr __impl::unpack_t unpack { };

}
}