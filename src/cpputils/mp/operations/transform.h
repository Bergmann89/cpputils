#pragma once

#include <cpputils/mp/misc/when.h>
#include <cpputils/mp/misc/make.h>
#include <cpputils/mp/misc/tag_of.h>
#include <cpputils/mp/misc/default.h>
#include <cpputils/mp/operations/unpack.h>

namespace utl {
namespace mp {

    namespace __impl
    {
        struct transform_t
        {
            template<typename Xs, typename F>
            constexpr auto operator()(Xs&& xs, F&& f) const;
        };
    }

    constexpr __impl::transform_t transform { };

    namespace __impl
    {
        template<typename Xs, typename = void>
        struct transform_impl
            : transform_impl<Xs, when<true>>
            { };

        template<typename T, bool condition>
        struct transform_impl<T, when<condition>>
            : default_
        {
            template<typename Xs, typename F>  
            static constexpr auto apply(Xs&& xs, F&& f) = delete;
        };

        template<typename T>
        struct transform_impl<T, when<!is_default<unpack_impl<tag_of<T>>>::value>>
        {
            using tag_type = tag_of<T>;

            template<typename F>
            struct transformer
            {
                F f;

                template<typename... Xn>
                constexpr auto operator()(Xn&&... xn) const
                    { return make<tag_type>((*f)(std::forward<Xn>(xn))...); }
            };

            template<typename Xs, typename F>
            static constexpr auto apply(Xs&& xs, F&& f)
                { return unpack(std::forward<Xs>(xs), transformer<decltype(&f)> { &f }); }
        };

        template<typename Xs, typename F>
        constexpr auto transform_t::operator()(Xs&& xs, F&& f) const
        {
            using tag_type = tag_of<Xs>;
            using transform_impl_type = transform_impl<tag_type>;
            return transform_impl_type::apply(std::forward<Xs>(xs), std::forward<F>(f));
        }
    }

}
}