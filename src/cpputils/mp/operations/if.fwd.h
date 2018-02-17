#pragma once

namespace utl {
namespace mp {

    namespace __impl
    {
        struct if_t
        {
            template <typename Cond, typename Then, typename Else>
            constexpr auto operator()(Cond&& c, Then&& t, Else&& e) const;
        };
    }

    constexpr __impl::if_t if_ { };

}
}