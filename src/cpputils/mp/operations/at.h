#pragma once

#include <utility>
#include <cpputils/mp/core/const.h>
#include <cpputils/mp/misc/when.h>
#include <cpputils/mp/misc/tag_of.h>
#include <cpputils/mp/misc/default.h>

namespace utl {
namespace mp {

    namespace __impl
    {
        struct at_t
        {
            template<typename Xs, typename N>
            constexpr auto operator()(Xs&& xs, const N& n) const;
        };
    }

    constexpr __impl::at_t at { };

    template<size_t N, typename Xs>
    constexpr auto at_c(Xs&& xs)
        { return at(std::forward<Xs>(xs), c_size_t<N> { }); }

    namespace __impl
    {
        template <typename T, typename = void>
        struct at_impl 
            : at_impl<T, when<true>>
            { };

        template <typename T, bool condition>
        struct at_impl<T, when<condition>>
            : default_
        {
            template <typename ...Args>
            static constexpr auto apply(Args&& ...) = delete;
        };

        template<typename Xs, typename N>
        constexpr auto at_t::operator()(Xs&& xs, const N& n) const
        {
            using tag_type     = tag_of<Xs>;
            using at_impl_type = at_impl<tag_type>;
            return at_impl_type::apply(std::forward<Xs>(xs), n);
        }
    }

}
}