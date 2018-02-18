#pragma once

#include <cpputils/mp/core/const.h>
#include <cpputils/mp/misc/when.h>
#include <cpputils/mp/misc/tag_of.h>
#include <cpputils/mp/misc/default.h>
#include <cpputils/mp/operations/unpack.fwd.h>

namespace utl {
namespace mp {

    namespace __impl
    {
        struct length_t
        {
            template<typename Xs>
            constexpr auto operator()(const Xs& xs) const;
        };
    }

    constexpr __impl::length_t length { };

    namespace __impl
    {
        struct length_argn
        {
            template<typename... Xn>
            constexpr c_size_t<sizeof...(Xn)> operator()(const Xn& ...) const
                { return { }; }
        };

        template <typename T, typename = void>
        struct length_impl
            : length_impl<T, when<true>>
            { };

        template<typename T, bool condition>
        struct length_impl<T, when<condition>>
            : default_
        {
            template<typename Xs>
            static constexpr auto apply(const Xs& xs)
                { return unpack(xs, length_argn { }); }
        };

        template<typename Xs>
        constexpr auto length_t::operator()(const Xs& xs) const
        {
            using tag_type         = tag_of<Xs>;
            using length_impl_type = length_impl<tag_type>;
            return length_impl_type::apply(xs);
        }
    }

}
}