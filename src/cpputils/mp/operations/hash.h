#pragma once

#include <cpputils/mp/misc/when.h>
#include <cpputils/mp/misc/tag_of.h>
#include <cpputils/mp/misc/default.h>
#include <cpputils/mp/core/const.h>
#include <cpputils/mp/core/modifier.h>
#include <cpputils/mp/core/conditionals.h>
#include <cpputils/mp/container/type.h>
#include <cpputils/mp/operations/hash.fwd.h>

namespace utl {
namespace mp {

    namespace __impl /* implementation */
    {
        template <typename T, typename = void>
        struct hash_integral_helper;

        template <typename Member, typename T>
        struct hash_integral_helper<Member T::*>
        {
            template <typename X>
            static constexpr auto apply(X const&)
                { return type_c<integral_constant<Member T::*, X::value>>; }
        };

        template <typename T>
        struct hash_integral_helper<T, enable_if<is_signed<T>>>
        {
            template <typename X>
            static constexpr auto apply(X const&)
            {
                constexpr signed long long x = X::value;
                return type_c<integral_constant<signed long long, x>>;
            }
        };

        template <typename T>
        struct hash_integral_helper<T, enable_if<is_unsigned<T>>>
        {
            template <typename X>
            static constexpr auto apply(X const&)
            {
                constexpr unsigned long long x = X::value;
                return type_c<integral_constant<unsigned long long, x>>;
            }
        };

        template <>
        struct hash_integral_helper<bool>
        {
            template <typename X>
            static constexpr auto apply(X const&)
                { return type_c<c_bool_t<X::value>>; }
        };

        template <>
        struct hash_integral_helper<char>
        {
            template <typename X>
            static constexpr auto apply(X const&)
            {
                using T = if_c<is_signed<char>::value, signed long long, unsigned long long>;
                constexpr T x = X::value;
                return type_c<integral_constant<T, x>>;
            }
        };

        template <typename T, typename>
        struct hash_impl
            : hash_impl<T, when<true>>
            { };

        template <typename Tag, bool condition>
        struct hash_impl<Tag, when<condition>>
            : default_
        {
            template <typename X>
            static constexpr auto apply(X const&) = delete;
        };

        template <typename T>
        struct hash_impl<tag_integral_constant<T>, when<true>>
        {
            template<typename X>
            static constexpr auto apply(const X& x)
            {
                using type = remove_cv<decltype(X::value)>;
                return hash_integral_helper<type>::apply(x);
            }
        };

        template<typename X>
        constexpr auto hash_t::operator()(const X& x) const
        {
            using tag_type = tag_of<X>;
            using hash_impl_type = hash_impl<tag_type>;
            return hash_impl_type::apply(x);
        }
    }

}
}