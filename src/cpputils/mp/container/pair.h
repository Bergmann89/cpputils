#pragma once

#include <cpputils/mp/core.h>
#include <cpputils/mp/misc/make.h>
#include <cpputils/mp/misc/tag_of.h>
#include <cpputils/mp/intern/ebo.h>
#include <cpputils/mp/intern/operators_orderable.h>
#include <cpputils/mp/intern/operators_comparable.h>
#include <cpputils/mp/operations/first.h>
#include <cpputils/mp/operations/second.h>
#include <cpputils/mp/operations/compare/less.h>
#include <cpputils/mp/operations/compare/less_equal.h>
#include <cpputils/mp/operations/compare/equal.h>
#include <cpputils/mp/operations/logical/or.h>
#include <cpputils/mp/operations/logical/and.h>
#include <cpputils/mp/operations/logical/not.h>

namespace utl {
namespace mp {

    struct tag_pair { };

    constexpr auto make_pair = make<tag_pair>;

    namespace __impl
    {
        template<int>
        struct pair_index;
    }

    template<typename First, typename Second>
    struct pair :
        private intern::ebo<__impl::pair_index<0>, First>,
        private intern::ebo<__impl::pair_index<1>, Second>
    {
        using tag       = tag_pair;
        using this_type = pair<First, Second>;

        // default constructor
        template<typename... dummy, typename = enable_if_c<
            is_constructible<First, dummy...>::value &&
            is_constructible<Second, dummy...>::value>>
        constexpr pair()
            : intern::ebo<__impl::pair_index<0>, First>()
            , intern::ebo<__impl::pair_index<1>, Second>()
            { }

        // variadic constructors
        template<typename... dummy, typename = enable_if_c<
            is_constructible<First, const First&, dummy...>::value &&
            is_constructible<Second, const Second&, dummy...>::value>>
        constexpr pair(First f, Second s)
            : intern::ebo<__impl::pair_index<0>, First>(f)
            , intern::ebo<__impl::pair_index<1>, Second>(s)
            { }

        template<typename F, typename S, typename = enable_if_c<
            is_convertible<F&&, First>::value &&
            is_convertible<S&&, Second>::value>>
        constexpr pair(F&& f, S&& s)
            : intern::ebo<__impl::pair_index<0>, First>(std::forward<F>(f))
            , intern::ebo<__impl::pair_index<1>, Second>(std::forward<S>(s))
            { }

        // copy and move constructor
        template<typename F, typename S, typename = enable_if_c<
            is_constructible<First, const F&>::value &&
            is_constructible<Second, const S&>::value &&
            is_convertible<const F&, First>::value &&
            is_convertible<const S&, Second>::value>>
        constexpr pair(const pair<F, S>& other)
            : intern::ebo<__impl::pair_index<0>, First>(mp::first(other))
            , intern::ebo<__impl::pair_index<1>, Second>(mp::second(other))
            { }

        template<typename F, typename S, typename = enable_if_c<
            is_constructible<First, const F&>::value &&
            is_constructible<Second, const S&>::value &&
            is_convertible<const F&, First>::value &&
            is_convertible<const S&, Second>::value>>
        constexpr pair(pair<F, S>&& other)
            : intern::ebo<__impl::pair_index<0>, First>(mp::first(std::forward<pair<F, S>>(other)))
            , intern::ebo<__impl::pair_index<1>, Second>(mp::second(std::forward<pair<F, S>>(other)))
            { }

        // copy and move assignment
        template <typename F, typename S, typename = enable_if_c<
            is_assignable<First&, const F&>::value &&
            is_assignable<Second&, const S&>::value>>
        constexpr pair& operator=(const pair<F, S>& other) {
            mp::first(*this)  = mp::first(other);
            mp::second(*this) = mp::second(other);
            return *this;
        }

        template <typename F, typename S, typename = enable_if_c<
            is_assignable<First&, const F &>::value &&
            is_assignable<Second&, const S &>::value>>
        constexpr pair& operator=(pair<F, S>&& other) {
            mp::first(*this)  = mp::first(std::move<pair<F, S>>(other));
            mp::second(*this) = mp::second(std::move<pair<F, S>>(other));
            return *this;
        }

        constexpr auto first() const
            { return mp::first(*this); }

        constexpr auto second() const
            { return mp::second(*this); }

        // Prevent the compiler from defining the default copy and move
        // constructors, which interfere with the SFINAE above.
        ~pair() = default;

        friend struct __impl::first_impl<tag_pair>;
        friend struct __impl::second_impl<tag_pair>;
    };

    namespace intern {

        template<>
        struct operators_comparable<tag_pair>
            { static constexpr bool value = true; };

        template<>
        struct operators_orderable<tag_pair>
            { static constexpr bool value = true; };

    }

    namespace __impl
    {
        /* tag_of */
        template<typename First, typename Second>
        struct tag_of_impl<pair<First, Second>>
            { using type = tag_pair; };

        /* make */
        template<>
        struct make_impl<tag_pair>
        {
            template<typename F, typename S>
            static constexpr pair<decay_type<F>, decay_type<S>> apply(F&& f, S&& s)
                { return { static_cast<F&&>(f), static_cast<S&&>(s) }; }
        };

        /* first */
        template<>
        struct first_impl<tag_pair>
        {
            using type = int;

            template<typename First, typename Second>
            static constexpr auto apply(pair<First, Second>& p)
            {
                return intern::ebo_get<pair_index<0>>(
                    static_cast<intern::ebo<pair_index<0>, First>&>(p)
                );
            }

            template<typename First, typename Second>
            static constexpr auto apply(pair<First, Second>&& p)
            {
                return intern::ebo_get<pair_index<0>>(
                    static_cast<intern::ebo<pair_index<0>, First>&&>(p)
                );
            }

            template<typename First, typename Second>
            static constexpr auto apply(const pair<First, Second>& p)
            {
                return intern::ebo_get<pair_index<0>>(
                    static_cast<const intern::ebo<pair_index<0>, First>&>(p)
                );
            }
        };

        /* second */
        template<>
        struct second_impl<tag_pair>
        {
            template<typename First, typename Second>
            static constexpr auto apply(pair<First, Second>& p)
            {
                return intern::ebo_get<pair_index<1>>(
                    static_cast<intern::ebo<pair_index<1>, Second>&>(p)
                );
            }

            template<typename First, typename Second>
            static constexpr auto apply(pair<First, Second>&& p)
            {
                return intern::ebo_get<pair_index<1>>(
                    static_cast<intern::ebo<pair_index<1>, Second>&&>(p)
                );
            }

            template<typename First, typename Second>
            static constexpr auto apply(const pair<First, Second>& p)
            {
                return intern::ebo_get<pair_index<1>>(
                    static_cast<const intern::ebo<pair_index<1>, Second>&>(p)
                );
            }
        };

        /* equal */
        template<>
        struct equal_impl<tag_pair, tag_pair>
        {
            template<typename L, typename R>
            static constexpr auto apply(const L& l, const R& r)
            {
                return and_(
                    equal(first(l), first(r)),
                    equal(second(l), second(r)));
            }
        };

        /* less */
        template<>
        struct less_impl<tag_pair, tag_pair>
        {
            template<typename L, typename R>
            static constexpr auto apply(const L& l, const R& r)
            {
                return or_(
                    less(first(l), first(r)),
                    and_(
                        less_equal(first (l), first (r)),
                        less      (second(l), second(r))
                    )
                );
            }
        };
    }

}
}