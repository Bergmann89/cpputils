#pragma once

#include <utility>
#include <cpputils/mp/misc/tag_of.h>
#include <cpputils/mp/misc/default.h>
#include <cpputils/mp/operations/at.h>
#include <cpputils/mp/operations/length.h>
#include <cpputils/mp/operations/unpack.fwd.h>

namespace utl {
namespace mp {

    namespace __impl
    {
        template<typename T, typename = void>
        struct unpack_impl
            : unpack_impl<T, when<true>>
            { };

        template<typename T>
        struct unpack_impl<T, when<
            !is_default<length_impl<tag_of<T>>>::value &&
            !is_default<at_impl    <tag_of<T>>>::value>>
        {
            template<typename Xs, typename F, size_t... I>
            static constexpr auto unpack_helper(Xs&& xs, F&& f, std::index_sequence<I...>)
                { return std::forward<F>(f)(at_c<I>(std::forward<Xs>(xs))...); }

            template<typename Xs, typename F>
            static constexpr auto apply(Xs&& xs, F&& f)
            {
                constexpr size_t N = decltype(length(xs))::value;
                return unpack_helper(std::forward<Xs>(xs), std::forward<F>(f), std::make_index_sequence<N> { });
            }
        };

        template<typename T, size_t N>
        struct unpack_impl<T[N]>
        {
            template<typename Xs, typename F, size_t... I>
            static constexpr auto unpack_helper(Xs&& xs, F&& f, std::index_sequence<I...>) 
                { return std::forward<F>(f)(std::forward<Xs>(xs)[I]...); }
            
            template<typename Xs, typename F>
            static constexpr auto apply(Xs&& xs, F&& f)
                { return unpack_helper(std::forward<Xs>(xs), std::forward<F>(f), std::make_index_sequence<N> { }); }
        };

        // TODO: Pair, Struct

        template<typename Xs, typename F>
        constexpr auto unpack_t::operator()(Xs&& xs, F&& f) const
        {
            using tag_type         = tag_of<Xs>;
            using unpack_impl_type = unpack_impl<tag_type>;
            return unpack_impl_type::apply(std::forward<Xs>(xs), std::forward<F>(f));
        }
    }

}
}