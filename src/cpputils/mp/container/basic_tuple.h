#pragma once

#include <utility>
#include <cpputils/mp/core/modifier.h>
#include <cpputils/mp/core/conditionals.h>
#include <cpputils/mp/misc/make.h>
#include <cpputils/mp/misc/tag_of.h>
#include <cpputils/mp/intern/ebo.h>
#include <cpputils/mp/operations/at.h>
#include <cpputils/mp/operations/unpack.h>
#include <cpputils/mp/operations/transform.h>

namespace utl {
namespace mp {

    struct tag_basic_tuple { };

    namespace __impl 
    {
        template<size_t>
        struct basic_tuple_index;

        template<typename Indices, typename... Xn>
        struct basic_tuple_impl;

        struct basic_tuple_from_other { };

        template<size_t... N, typename... Xn>
        struct basic_tuple_impl<std::index_sequence<N...>, Xn...>
            : intern::ebo<basic_tuple_index<N>, Xn>...
        {
            static constexpr size_t _size = sizeof...(Xn);

            constexpr basic_tuple_impl() = default;

            template<typename Other>
            explicit constexpr basic_tuple_impl(basic_tuple_from_other, Other&& other)
                : intern::ebo<basic_tuple_index<N>, Xn>(intern::ebo_get<basic_tuple_index<N>>(std::forward<Other>(other)))...
                { }

            template<typename... Yn>
            explicit constexpr basic_tuple_impl(Yn&&... yn) 
                : intern::ebo<basic_tuple_index<N>, Xn>(std::forward<Yn>(yn))...
                { }
        };
    }

    template<typename... Xn>
    struct basic_tuple final
        : __impl::basic_tuple_impl<std::make_index_sequence<sizeof...(Xn)>, Xn...>
    {
        using tag       = tag_basic_tuple;
        using base_type = __impl::basic_tuple_impl<std::make_index_sequence<sizeof...(Xn)>, Xn...>;

        constexpr basic_tuple() = default;

        template<typename Other, typename = enable_if<is_same<basic_tuple, decay_type<Other>>>>
        constexpr basic_tuple(Other&& other)
            : base_type(__impl::basic_tuple_from_other { }, std::forward<Other>(other))
            { }

        template<typename... Yn>
        explicit constexpr basic_tuple(Yn&&... yn)
            : base_type(std::forward<Yn>(yn)...)
            { }

        template<typename N>
        constexpr auto at(const N& n) const
            { return mp::at(*this, n); }

        template<typename F>
        constexpr auto unpack(F&& f) const
            { return mp::unpack(*this, std::forward<F>(f)); }
        
        template<typename F>
        constexpr auto transform(F&& f) const
            { return mp::transform(*this, std::forward<F>(f)); }
    };

    constexpr auto make_basic_tuple = make<tag_basic_tuple>;

    namespace __impl
    {
        /* tag_of */
        template<typename... Xn>
        struct tag_of_impl<basic_tuple<Xn...>>
            { using type = tag_basic_tuple; };

        /* make */
        template<>
        struct make_impl<tag_basic_tuple>
        {
            template<typename... Xn>
            static constexpr auto apply(Xn&&... xn)
                { return basic_tuple<decay_type<Xn>...> { std::forward<Xn>(xn)... }; }
        };

        /* unpack */
        template<>
        struct unpack_impl<tag_basic_tuple>
        {
            template<size_t... I, typename... Xn, typename F>
            static constexpr auto apply(const basic_tuple_impl<std::index_sequence<I...>, Xn...>& xs, F&& f)
            {
                return std::forward<F>(f)(
                    intern::ebo_get<basic_tuple_index<I>>(
                        static_cast<const intern::ebo<basic_tuple_index<I>, Xn>&>(xs)
                    )...
                );
            }

            template<size_t... I, typename... Xn, typename F>
            static constexpr auto apply(basic_tuple_impl<std::index_sequence<I...>, Xn...>& xs, F&& f)
            {
                return std::forward<F>(f)(
                    intern::ebo_get<basic_tuple_index<I>>(
                        static_cast<intern::ebo<basic_tuple_index<I>, Xn>&>(xs)
                    )...
                );
            }

            template<size_t... I, typename... Xn, typename F>
            static constexpr auto apply(basic_tuple_impl<std::index_sequence<I...>, Xn...>&& xs, F&& f)
            {
                return std::forward<F>(f)(
                    intern::ebo_get<basic_tuple_index<I>>(
                        static_cast<intern::ebo<basic_tuple_index<I>, Xn>&&>(xs)
                    )...
                );
            }
        };

        /* at */
        template<>
        struct at_impl<tag_basic_tuple>
        {
            template<typename Xs, typename N>
            static constexpr auto apply(Xs&& xs, const N&)
            {
                constexpr size_t index = N::value;
                return intern::ebo_get<basic_tuple_index<index>>(std::forward<Xs>(xs));
            }
        };
    }

    template <size_t N, typename... Xn>
    constexpr auto at_c(const basic_tuple<Xn...>& x)
        { return intern::ebo_get<__impl::basic_tuple_index<N>>(x); }

    template <size_t N, typename... Xn>
    constexpr auto at_c(basic_tuple<Xn...>& x)
        { return intern::ebo_get<__impl::basic_tuple_index<N>>(x); }

    template <size_t N, typename... Xn>
    constexpr auto at_c(basic_tuple<Xn...>&& x)
        { return intern::ebo_get<__impl::basic_tuple_index<N>>(std::forward<basic_tuple<Xn...>>(x)); }

}
}