#pragma once

#include <utility>
#include <cpputils/mp/misc/make.h>
#include <cpputils/mp/core/const.h>
#include <cpputils/mp/core/modifier.h>
#include <cpputils/mp/operations/hash.fwd.h>
#include <cpputils/mp/intern/operators_comparable.h>

namespace utl {
namespace mp {

    namespace __impl /* forward declaration */
    {
        template<typename T>
        struct type_impl;

        struct decl_type_t
        {
            template<typename T>
            constexpr auto operator()(T&& t) const;
        };

        struct type_id_t
        {
            template <typename T>
            constexpr auto operator()(T&&) const;
        };

        struct size_of_t
        {
            template <typename T>
            constexpr auto operator()(T&&) const;
        };

        struct align_of_t
        {
            template <typename T>
            constexpr auto operator()(T&&) const;
        };

        struct is_valid_type_t
        {
            template <typename F>
            constexpr auto operator()(F&&) const;

            template <typename F, typename ...Args>
            constexpr auto operator()(F&&, Args&&...) const;
        };

        template <template <typename ...> class F>
        struct tmplt_t;

        template <template <typename ...> class F>
        struct metafunction_t;

        template <typename F>
        struct metafunction_class_t
            : metafunction_t<F::template apply>
            { };

        template <typename F>
        struct integral_t;

        struct make_integral_t
        {
            template <typename F>
            constexpr integral_t<F> operator()(F const&) const
                { return { }; }
        };
    }

    struct tag_type { };

    template<typename T>
    using type = typename __impl::type_impl<T>::type;

    template<typename T>
    constexpr type<T> type_c { };

    constexpr auto make_mp_type = make<tag_type>;

    constexpr __impl::decl_type_t decl_type { };

    constexpr __impl::type_id_t type_id { };

    constexpr __impl::size_of_t size_of { };

    constexpr __impl::align_of_t align_of { };

    constexpr __impl::is_valid_type_t is_valid_type { };

    template <template <typename ...> class F>
    constexpr __impl::tmplt_t<F> tmplt { };

    template <template <typename ...> class F>
    constexpr __impl::metafunction_t<F> metafunction { };

    template <typename F>
    constexpr __impl::metafunction_class_t<F> metafunction_class { };

    constexpr __impl::make_integral_t integral { };

    template <template <typename ...> class F>
    constexpr auto trait = integral(metafunction<F>);

    namespace __impl /* implementation */
    {
        /* basic_type */
        template<typename T>
        struct basic_type
        {
            using tag  = tag_type;
            using type = T;

            constexpr auto operator+() const
                { return *this; }
        };

        /* type */
        template<typename T>
        struct type_impl
        { 
            struct type : basic_type<T> 
                { };
        };

        /* decl_type */
        template <typename T, typename = tag_type>
        struct decltype_helper
            { using type = remove_ref<T>; };

        template <typename T>
        struct decltype_helper<T, tag_of<T>>
            { using type = typename remove_ref<T>::type; };

        template<typename T>
        constexpr auto decl_type_t::operator()(T&& t) const
            { return type_c<typename decltype_helper<T>::type>; }
        
        /* type_id */
        template <typename T, typename = tag_type>
        struct typeid_helper
            { using type = clean_type<T>; };

        template <typename T>
        struct typeid_helper<T, tag_of<T>>
            { using type = typename remove_ref<T>::type; };

        template <typename T>
        constexpr auto type_id_t::operator()(T&&) const
            { return type_c<typename typeid_helper<T>::type>; }

        /* make */
        template <>
        struct make_impl<tag_type>
        {
            template <typename T>
            static constexpr auto apply(T&& t)
                { return type_id(std::forward<T>(t)); }
        };

        /* size_of */
        template <typename T>
        constexpr auto size_of_t::operator()(T&&) const
            { return c_size_t<sizeof(typename decltype_helper<T>::type)> { }; }
        
        /* align_of */
        template <typename T>
        constexpr auto align_of_t::operator()(T&&) const
            { return c_size_t<alignof(typename decltype_helper<T>::type)> { }; }
        
        /* is_valid_type */
        template<typename F, typename... Args, typename = decltype(std::declval<F&&>()(std::declval<Args&&>()...))>
        constexpr auto is_valid_type_impl(int)
            { return c_true; }

        template<typename F, typename... Args>
        constexpr auto is_valid_type_impl(...)
            { return c_false; }

        template<typename F>
        struct is_valid_type_helper
        {
            template<typename... Args>
            constexpr auto operator()(Args&&...) const
                { return is_valid_type_impl<F, Args&&...>(int { }); }
        };

        template <typename F>
        constexpr auto is_valid_type_t::operator()(F&&) const
            { return is_valid_type_helper<F&&> { }; }

        template <typename F, typename ...Args>
        constexpr auto is_valid_type_t::operator()(F&&, Args&&...) const
            { return is_valid_type_impl<F&&, Args&&...>(int { }); }

        /* tmplt */
        template <template <typename ...> class F>
        struct tmplt_t
        {
            template<typename... T>
            struct apply
                { using type = F<T...>; };
            
            template<typename... T>
            constexpr auto operator()(const T&...) const
                { return type<F<typename T::type...>> { }; }
        };

        /* metafunction */
        template <template <typename ...> class F>
        struct metafunction_t
        {
            template<typename... T>
            using apply = F<T...>;

            template<typename... T>
            constexpr type<typename F<typename T::type...>::type> operator()(const T&...) const
                { return { }; }
        };

        /* integral */
        template <typename F>
        struct integral_t
        {
            template<typename... T>
            constexpr auto operator()(const T& ...) const
            {
                using result_type = typename F::template apply<typename T::type...>::type;
                return result_type { };
            }
        };

        /* equal */
        template <>
        struct equal_impl<tag_type, tag_type>
        {
            template <typename T, typename U>
            static constexpr auto apply(const basic_type<T>&, const basic_type<U>&)
                { return c_false; }

            template <typename T>
            static constexpr auto apply(const basic_type<T>&, const basic_type<T>&)
                { return c_true; }
        };

        /* hash */
        template<>
        struct hash_impl<tag_type>
        {
            template <typename T>
            static constexpr T apply(const T& t)
                { return t; }
        };

    }

    namespace intern {

        template<>
        struct operators_comparable<tag_type>
            { static constexpr bool value = true; };

    }

}
}