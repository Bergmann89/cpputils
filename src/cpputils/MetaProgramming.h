#pragma once

#include <tuple>
#include <string>
#include <cstdlib>
#include <cstdint>
#include <type_traits>

#define MP_DEBUG

#if defined __GNUC__
#   define LIKELY(EXPR)  __builtin_expect(!!(EXPR), 1)
#else
#   define LIKELY(EXPR)  (!!(EXPR))
#endif

#if defined MP_DEBUG
#   define X_ASSERT(CHECK) void(0)
#else
#   define X_ASSERT(CHECK) ( LIKELY(CHECK) ?  void(0) : []{assert(!#CHECK);}() )
#endif

namespace utl
{

    /* types */
    template<class T, T t>
    using mp_const = std::integral_constant<T, t>;

    template<bool B>
    using mp_bool = mp_const<bool, B>;

    template<size_t S>
    using mp_size_t = mp_const<size_t, S>;

    template<class...>
    struct mp_list { };

    template<class T>
    struct mp_identity
        { using type = T; };

    template<class... T>
    struct mp_inherit : T...
        { };

    template<class T, T... Ints>
    struct mp_integer_sequence
        { };

    template<size_t... Ints>
    using mp_index_sequence = mp_integer_sequence<size_t, Ints...>;



    /* constants */
    using mp_true    = mp_bool<true>;
    using mp_false   = mp_bool<false>;
    using mp_zero    = mp_size_t<0>;



    /* modifier */
    template<class T>
    using mp_add_pointer = T*;

    template<class T>
    using mp_add_reference = T&;

    template<class T>
    using mp_remove_ptr = typename std::remove_pointer<T>::type;

    template<class T>
    using mp_remove_const = typename std::remove_const<T>::type;

    template<class T>
    using mp_remove_cv = typename std::remove_cv<T>::type;

    template<class T>
    using mp_remove_ref = typename std::remove_reference<T>::type;

    template<class T>
    using mp_clean_type = mp_remove_cv<mp_remove_ptr<mp_remove_ref<T>>>;



    /* conditionals */
    template<class T, class S>
    using mp_is_same = mp_bool<std::is_same<T, S>::value>;

    template<class Base, class Derived>
    using mp_is_base_of = mp_bool<std::is_base_of<Base, Derived>::value>;

    template<class T, class S = void>
    using mp_enable_if = typename std::enable_if<T::value, S>::type;

    template<bool B, class S = void>
    using mp_enable_if_c = typename std::enable_if<B, S>::type;



    namespace __impl
    {
        /* logical operations */
        template<bool C, class T, class E>
        struct mp_if_c_impl;

        template<bool C, class T, template<class...> class E, class... A>
        struct mp_eval_if_c_impl;

        template<class L>
        struct mp_not_impl;

        template<class L>
        struct mp_and_impl;

        template<class L>
        struct mp_or_impl;



        /* arithmetic */
        template<class L>
        struct mp_plus_impl;

        template<class L>
        struct mp_minus_impl;

        template<class L>
        struct mp_multiply_impl;

        template<class L>
        struct mp_divide_impl;



        /* generator functions */
        template<class S>
        struct next_integer_sequence;

        template<class T, T I, T N>
        struct mp_make_int_seq_impl;

        template<size_t N, class... T>
        struct mp_repeat_impl;



        /* list operations returning a numeric value */
        template<class L>
        struct mp_size_impl;

        template<class L, class V>
        struct mp_count_impl;

        template<class L, template<class...> class P>
        struct mp_count_if_impl;



        /* operations to manipulate list content */
        template<class L, class... T>
        struct mp_push_front_impl;

        template<class L>
        struct mp_pop_front_impl;

        template<class L, class... T>
        struct mp_push_back_impl;

        template<class... L>
        struct mp_concat_impl;

        template<class L, template<class> class F>
        struct mp_filter_impl;



        /* list operations returning a bool value */
        template<class... L>
        struct mp_empty_impl;

        template<class L, class V>
        struct mp_contains_impl;



        /* list operations returing elements from the list */
        template<class L>
        struct mp_front_impl;

        template<class L>
        struct mp_second_impl;

        template<class L, size_t I>
        struct mp_at_c_impl;

        template<class L, template<class> class F, class TDefault>
        struct mp_search_impl;

        template<class L, class V>
        struct mp_find_impl;

        template<class M, class K>
        struct mp_map_find_impl;



        /* list manipulators returning a new list */
        template<class L>
        struct mp_clear_impl;

        template<class L, class C>
        struct mp_unique_impl;

        template<class A, template<class...> class B>
        struct mp_rename_impl;

        template<class... L>
        struct mp_append_impl;

        template<class L, class S>
        struct mp_map_from_list_impl;

        template<template<class...> class F, class E, class... L>
        struct mp_transform_impl;



        /* unique id */
        template<class Counter>
        struct mp_unique_counter;

        template<class Counter, class... Args>
        struct mp_unique_id;



        /* strings */
        struct tag_reference { };
        struct tag_array { };

        template<size_t N, class T>
        struct mp_string_impl;

        template<class I, int Base>
        struct mp_int_to_string_impl;

        template<size_t N>
        using mp_array_string = mp_string_impl<N, tag_array>;

        template<class J, class L>
        struct mp_string_type_join_impl;



        /* tuple */
        template<bool, class F, class T, class... Args>
        struct mp_tuple_call_helper
        {
            static inline void exec(F f, T& t, Args&&... args)
                { }
        };

        template<class F, class T, class... Args>
        struct mp_tuple_call_helper<true, F, T, Args...>
        {
            static inline void exec(F f, T& t, Args&&... args)
                { f(t, std::forward<Args>(args)...); }
        };

        template<class T>
        using mp_tuple_filter = mp_true;

        template<template<class> class Filter, class T, class F, size_t... I, class... Args>
        void for_each_in_tuple_filter(T& t, F f, const mp_index_sequence<I...>&, Args&&... args)
        {
            using expander = int[];
            (void) f;
            (void) expander { (mp_tuple_call_helper<
                                    Filter<decltype(std::get<I>(t))>::value,
                                    F,
                                    decltype(std::get<I>(t)),
                                    Args...>::exec(
                                        f,
                                        std::get<I>(t),
                                        std::forward<Args>(args)...), 0)... };
        }
    }



    /* logical operations */
    template<class B, class T, class E>
    using mp_if = typename __impl::mp_if_c_impl<B::value != 0, T, E>::type;

    template<bool B, class T, class E>
    using mp_if_c = typename __impl::mp_if_c_impl<B, T, E>::type;

    template<class C, class T, template<class...> class E, class... A>
    using mp_eval_if = typename __impl::mp_eval_if_c_impl<C::value != 0, T, E, A...>::type;

    template<bool C, class T, template<class...> class E, class... A>
    using mp_eval_if_c = typename __impl::mp_eval_if_c_impl<C, T, E, A...>::type;

    template<class L>
    using mp_not = typename __impl::mp_not_impl<L>::type;

    template<class L>
    using mp_and = typename __impl::mp_and_impl<L>::type;

    template<class L>
    using mp_or = typename __impl::mp_or_impl<L>::type;



    /* arithmetic */
    template<class L>
    using mp_plus = typename __impl::mp_plus_impl<L>::type;

    template<class L>
    using mp_minus = typename __impl::mp_minus_impl<L>::type;

    template<class L>
    using mp_multiply = typename __impl::mp_multiply_impl<L>::type;

    template<class L>
    using mp_divide = typename __impl::mp_divide_impl<L>::type;



    /* generator functions */
    template<class T, T N>
    using mp_make_integer_sequence = typename __impl::mp_make_int_seq_impl<T, 0, N>::type;

    template<size_t N>
    using mp_make_index_sequence = mp_make_integer_sequence<size_t, N>;

    template<size_t N, class... T>
    using mp_repeat = typename __impl::mp_repeat_impl<N, T...>::type;



    /* list operations returning a numeric value */
    template<class... T>
    using mp_length = mp_const<size_t, sizeof...(T)>;

    template<class L>
    using mp_size = typename __impl::mp_rename_impl<L, mp_length>::type;

    template<class L, class V>
    using mp_count = typename __impl::mp_count_impl<L, V>::type;

    template<class L, template<class...> class P>
    using mp_count_if = typename __impl::mp_count_if_impl<L, P>::type;



    /* operations to manipulate list content */
    template<class L, class... T>
    using mp_push_front = typename __impl::mp_push_front_impl<L, T...>::type;

    template<class L, class... T>
    using mp_pop_front = typename __impl::mp_pop_front_impl<L>::type;

    template<class L, class... T>
    using mp_push_back = typename __impl::mp_push_back_impl<L, T...>::type;

    template<class... L>
    using mp_concat = typename __impl::mp_concat_impl<L...>::type;

    template<class L, template<class> class F>
    using mp_filter = typename __impl::mp_filter_impl<L, F>::type;



    /* list operations returning a bool value */
    template<class... L>
    using mp_empty = typename __impl::mp_empty_impl<L...>::type;

    template<class L, class V>
    using mp_contains = mp_bool<mp_count<L, V>::value != 0>;



    /* list operations returing elements from the list */
    template<class L>
    using mp_front = typename __impl::mp_front_impl<L>::type;

    template<class L>
    using mp_second = typename __impl::mp_second_impl<L>::type;

    template<class L, class I>
    using mp_at = typename __impl::mp_at_c_impl<L, I::value>::type;

    template<class L, size_t I>
    using mp_at_c = typename __impl::mp_at_c_impl<L, I>::type;

    template<class L, template<class> class F, class TDefault = void>
    using mp_search = typename __impl::mp_search_impl<L, F, TDefault>::type;

    template<class L, class V>
    using mp_find = typename __impl::mp_find_impl<L, V>::type;

    template<class M, class K>
    using mp_map_find = typename __impl::mp_map_find_impl<M, K>::type;



    /* list manipulators returning a new list */
    template<class L>
    using mp_clear = typename __impl::mp_clear_impl<L>::type;

    template<class L>
    using mp_unique = typename __impl::mp_unique_impl<L, mp_clear<L>>::type;

    template<class A, template<class...> class B>
    using mp_rename = typename __impl::mp_rename_impl<A, B>::type;

    template<class... L>
    using mp_append = typename __impl::mp_append_impl<L...>::type;

    template<class L>
    using mp_map_from_list = typename __impl::mp_map_from_list_impl<L, mp_make_index_sequence<mp_size<L>::value>>::type;

    template<template<class...> class F, class... L>
    using mp_transform = typename __impl::mp_transform_impl<F, mp_empty<L...>, L...>::type;



    /* unique id */
    template<class Counter>
    inline size_t nextUniqueId()
        { return __impl::mp_unique_counter<Counter>::next(); }

    template<class Counter, class... Args>
    inline size_t getUniqueId()
        { return __impl::mp_unique_id<Counter, Args...>::value(); }



    /* strings */
    template<size_t N>
    using mp_string = __impl::mp_string_impl<N, __impl::tag_reference>;

    template<class I, int Base>
    constexpr auto mp_int_to_string()
        { return __impl::mp_int_to_string_impl<I, Base>::value; }

    template <int N>
    constexpr mp_string<N - 1> mp_make_string(const char (&data)[N])
        { return mp_string<N - 1>(data); }

    template <size_t N1, size_t N2, class T1, class T2>
    constexpr __impl::mp_array_string<N1 + N2> mp_string_cat(
        const __impl::mp_string_impl<N1, T1>& s1,
        const __impl::mp_string_impl<N2, T2>& s2)
            { return __impl::mp_array_string<N1 + N2>(s1, s2); };

    template <size_t N1, size_t N2, class T1, class T2>
    constexpr auto operator + (
        const __impl::mp_string_impl<N1, T1>& s1,
        const __impl::mp_string_impl<N2, T2>& s2)
            { return mp_string_cat(s1, s2); };

    template<size_t NJ, size_t N1, size_t N2, class TJ, class T1, class T2>
    constexpr auto mp_string_join(
        const __impl::mp_string_impl<NJ, TJ>& j,
        const __impl::mp_string_impl<N1, T1>& s1,
        const __impl::mp_string_impl<N2, T2>& s2)
        { return mp_string_cat(s1, mp_string_cat(j, s2)); }

    template<size_t NJ, size_t N1, size_t... NX, class TJ, class T1, class... TX>
    constexpr auto mp_string_join(
        const __impl::mp_string_impl<NJ, TJ>& j,
        const __impl::mp_string_impl<N1, T1>& s,
        const __impl::mp_string_impl<NX, TX>&... rest)
        { return mp_string_cat(s, mp_string_cat(j, mp_string_join(j, rest...))); }

    template<class J, class L>
    using mp_string_type_join = __impl::mp_string_type_join_impl<J, L>;



    /* tuple */
    template<class... T, class F, class... Args>
    void mp_for_each_in_tuple(std::tuple<T...>& t, F f, Args&&... args)
        { __impl::for_each_in_tuple_filter<__impl::mp_tuple_filter>(t, f, mp_make_index_sequence<mp_length<T...>::value>(), std::forward<Args>(args)...); }

    template<class... T, class F, class... Args>
    void mp_for_each_in_tuple(const std::tuple<T...>& t, F f, Args&&... args)
        { __impl::for_each_in_tuple_filter<__impl::mp_tuple_filter>(t, f, mp_make_index_sequence<mp_length<T...>::value>(), std::forward<Args>(args)...); }

    template<template<class> class Filter, class... T, class F, class... Args>
    void mp_for_each_in_tuple_filter(std::tuple<T...>& t, F f, Args&&... args)
        { __impl::for_each_in_tuple_filter<Filter>(t, f, mp_make_index_sequence<mp_length<T...>::value>(), std::forward<Args>(args)...); }

    template<template<class> class Filter, class... T, class F, class... Args>
    void mp_for_each_in_tuple_filter(const std::tuple<T...>& t, F f, Args&&... args)
        { __impl::for_each_in_tuple_filter<Filter>(t, f, mp_make_index_sequence<mp_length<T...>::value>(), std::forward<Args>(args)...); }

    template<template<class...> class F>
    struct mp_for_each_proxy
    {
        template<class T, class... Args>
        inline void operator()(T& t, Args&&... args) const
            { F<mp_clean_type<T>>::exec(t, std::forward<Args>(args)...); }
    };



    namespace __impl
    {
        /* helper */
        constexpr size_t cx_plus()
            { return 0; }

        template<class T1, class... T>
        constexpr size_t cx_plus(T1 t1, T... t)
            { return t1 + cx_plus(t...); }

        constexpr bool cx_and()
            { return true; }

        template<class... T>
        constexpr bool cx_and(bool b, T... t)
            { return b && cx_and(t...); }

        constexpr bool cx_or()
            { return false; }

        template<class... T>
        constexpr bool cx_or(bool b, T... t)
            { return b || cx_or(t...); }

        constexpr size_t cx_find_index( bool const * first, bool const * last )
            { return first == last || *first ? 0 : 1 + cx_find_index(first + 1, last); }



        /* logical operations */
        template<class T, class F>
        struct mp_if_c_impl<true, T, F>
            { using type = T; };

        template<class T, class F>
        struct mp_if_c_impl<false, T, F>
            { using type = F; };

        template<class T, template<class...> class E, class... A>
        struct mp_eval_if_c_impl<true, T, E, A...>
            { using type = T; };

        template<class T, template<class...> class E, class... A>
        struct mp_eval_if_c_impl<false, T, E, A...>
            { using type = E<A...>; };

        template<template<class...> class L, class... T>
        struct mp_not_impl<L<T...>>
            { using type = L<mp_bool<T::value == 0>...>; };

        template<template<class...> class L, class... T>
        struct mp_and_impl<L<T...>>
            { using type = mp_bool<cx_and(mp_bool<T::value>::value...)>; };

        template<template<class...> class L, class... T>
        struct mp_or_impl<L<T...>>
            { using type = mp_bool<cx_or(mp_bool<T::value>::value...)>; };



        /* arithmetic */
        template<template<class...> class L, class T>
        struct mp_plus_impl<L<T>>
            { using type = T; };

        template<template<class...> class L, class T1, class... T>
        struct mp_plus_impl<L<T1, T...>>
        {
            using _type = mp_remove_const<decltype(T1::value)>;
            static constexpr _type _value = T1::value + mp_plus<L<T...>>::value;
            using type = mp_const<_type, _value>;
        };

        template<template<class...> class L, class T>
        struct mp_minus_impl<L<T>>
            { using type = T; };

        template<template<class...> class L, class T1, class... T>
        struct mp_minus_impl<L<T1, T...>>
        {
            using _type = mp_remove_const<decltype(T1::value)>;
            static constexpr _type _value = T1::value - mp_plus<L<T...>>::value;
            using type = mp_const<_type, _value>;
        };

        template<template<class...> class L, class T>
        struct mp_multiply_impl<L<T>>
            { using type = T; };

        template<template<class...> class L, class T1, class... T>
        struct mp_multiply_impl<L<T1, T...>>
        {
            using _type = mp_remove_const<decltype(T1::value)>;
            static constexpr _type _value = T1::value * mp_multiply<L<T...>>::value;
            using type = mp_const<_type, _value>;
        };

        template<template<class...> class L, class T>
        struct mp_divide_impl<L<T>>
            { using type = T; };

        template<template<class...> class L, class T1, class... T>
        struct mp_divide_impl<L<T1, T...>>
        {
            using _type = mp_remove_const<decltype(T1::value)>;
            static constexpr _type _value = T1::value / mp_multiply<L<T...>>::value;
            using type = mp_const<_type, _value>;
        };



        /* generator functions */
        template<class S>
        struct next_integer_sequence;

        template<class T, T... Ints>
        struct next_integer_sequence<mp_integer_sequence<T, Ints...>>
            { using type = mp_integer_sequence<T, Ints..., sizeof...(Ints)>; };

        template<class T, T I, T N>
        struct mp_make_int_seq_impl
            { using type = typename next_integer_sequence<typename mp_make_int_seq_impl<T, I+1, N>::type>::type; };

        template<class T, T N>
        struct mp_make_int_seq_impl<T, N, N>
            { using type = mp_integer_sequence<T>; };

        template<size_t N, class... T>
        struct mp_repeat_impl
        {
            using _l1 = typename mp_repeat_impl<N/2, T...>::type;
            using _l2 = typename mp_repeat_impl<N%2, T...>::type;
            using type = mp_append<_l1, _l1, _l2>;
        };

        template<class... T> struct mp_repeat_impl<0, T...>
            { using type = mp_list<>; };

        template<class... T> struct mp_repeat_impl<1, T...>
            { using type = mp_list<T...>; };



        /* list operations returning a numeric value */
        template<template<class...> class L, class... T>
        struct mp_size_impl<L<T...>>
            { using type = mp_length<T...>; };

        template<template<class...> class L, class... T, class V>
        struct mp_count_impl<L<T...>, V>
            { using type = mp_size_t<cx_plus(std::is_same<T, V>::value...)>; };

        template<template<class...> class L, class... T, template<class...> class P>
        struct mp_count_if_impl<L<T...>, P>
            { using type = mp_size_t<cx_plus(P<T>::value...)>; };



        /* operations to manipulate list content */
        template<template<class...> class L, class... U, class... T>
        struct mp_push_front_impl<L<U...>, T...>
            { using type = L<T..., U...>; };

        template<template<class...> class L, class T1, class... T>
        struct mp_pop_front_impl<L<T1, T...>>
                { using type = L<T...>; };

        template<template<class...> class L, class... U, class... T>
        struct mp_push_back_impl<L<U...>, T...>
            { using type = L<U..., T...>; };

        template<template<class...> class L1, template<class...> class L2, class... T1, class... T2>
        struct mp_concat_impl<L1<T1...>, L2<T2...>>
            { using type = L1<T1..., T2...>; };

        template<template<class...> class L1, template<class...> class L2, class... T1, class... T2, class... L>
        struct mp_concat_impl<L1<T1...>, L2<T2...>, L...>
        {
            using _rest = mp_concat<L2<T2...>, L...>;
            using type = mp_concat<L1<T1...>, _rest>;
        };

        template<template<class...> class L, template<class> class F>
        struct mp_filter_impl<L<>, F>
            { using type = L<>; };

        template<template<class...> class L, template<class> class F, class T1, class... T>
        struct mp_filter_impl<L<T1, T...>, F>
        {
            using _rest  = mp_filter<L<T...>, F>;
            using _match = F<T1>;
            using type   = mp_eval_if_c<_match::value == 0, _rest, mp_push_front, _rest, T1>;
        };



        /* list operations returning a bool value */
        template<template<class...> class L1, class... T>
        struct mp_empty_impl<L1<T...>>
            { using type = mp_bool<mp_length<T...>::value == 0>; };

        template<template<class...> class L1, class... T, class... L>
        struct mp_empty_impl<L1<T...>, L...>
        {
            static constexpr bool _first = !static_cast<bool>(mp_length<T...>::value);
            static constexpr bool _other =  static_cast<bool>(mp_empty<L...>::value);
            using type = mp_bool<_first && _other>;
        };



        /* list operations returing elements from the list */
        template<template<class...> class L, class T1, class... T>
        struct mp_front_impl<L<T1, T...>>
            { using type = T1; };

        template<template<class...> class L, class T1, class T2, class... T>
        struct mp_second_impl<L<T1, T2, T...>>
            { using type = T2; };

        template<class L, size_t I>
        struct mp_at_c_impl
        {
            using map = mp_map_from_list<L>;
            using type = mp_second<mp_map_find<map, mp_size_t<I>>>;
        };

        template<bool B, class L, template<class> class F, class TDefault>
        struct mp_search_helper;

        template<template<class...> class L, template<class> class F, class TDefault, class T1, class... T>
        struct mp_search_helper<true, L<T1, T...>, F, TDefault>
            { using type = T1; };

        template<template<class...> class L, template<class> class F, class TDefault, class T1, class... T>
        struct mp_search_helper<false, L<T1, T...>, F, TDefault>
            { using type = mp_search<L<T...>, F, TDefault>; };

        template<template<class...> class L, template<class> class F, class TDefault>
        struct mp_search_impl<L<>, F, TDefault>
            { using type = TDefault; };

        template<template<class...> class L, template<class> class F, class TDefault, class T1, class... T>
        struct mp_search_impl<L<T1, T...>, F, TDefault>
        {
            using c_value = F<T1>;
            using type    = typename mp_search_helper<c_value::value, L<T1, T...>, F, TDefault>::type;
        };

        template<template<class...> class L, class V>
        struct mp_find_impl<L<>, V>
            { using type = mp_zero; };

        template<template<class...> class L, class... T, class V>
        struct mp_find_impl<L<T...>, V>
        {
            static constexpr bool _v[] = { std::is_same<T, V>::value... };
            using type = mp_size_t<cx_find_index(_v, _v + sizeof...(T))>;
        };

        template<template<class...> class M, class... T, class K>
        struct mp_map_find_impl<M<T...>, K>
        {
            static mp_identity<void> f( ... );

            template<template<class...> class L, class... U>
            static mp_identity<L<K, U...>>
                f( mp_identity<L<K, U...>>* );

            using U    = mp_inherit<mp_identity<T>...>;
            using V    = decltype( f((U*)0) );
            using type = typename V::type;
        };



        /* list manipulators returning a new list */
        template<template<class...> class L, class... T>
        struct mp_clear_impl<L<T...>>
            { using type = L<>; };

        template<template<class...> class L, class... T>
        struct mp_unique_impl<L<>, L<T...>>
            { using type = L<T...>; };

        template<template<class...> class L, class C, class T1, class... T>
        struct mp_unique_impl<L<T1, T...>, C>
        {
            using type = mp_if<
                mp_contains<C, T1>,
                typename mp_unique_impl<L<T...>, C>::type,
                typename mp_unique_impl<L<T...>, mp_push_back<C, T1>>::type
            >;
        };

        template<template<class...> class A, class... T, template<class...> class B>
        struct mp_rename_impl<A<T...>, B>
            { using type = B<T...>; };

        template<>
        struct mp_append_impl<>
            { using type = mp_list<>; };

        template<template<class...> class L, class... T>
        struct mp_append_impl<L<T...>>
            { using type = L<T...>; };

        template<
            template<class...> class L1, class... T1,
            template<class...> class L2, class... T2, class... Lr>
        struct mp_append_impl<L1<T1...>, L2<T2...>, Lr...>
            { using type = mp_append<L1<T1..., T2...>, Lr...>; };

        template<template<class...> class L, class... T, size_t... J>
        struct mp_map_from_list_impl<L<T...>, mp_integer_sequence<size_t, J...>>
            { using type = mp_list<mp_list<mp_size_t<J>, T>...>; };

        template<template<class...> class F, class L1, class... L>
        struct mp_transform_impl<F, mp_true, L1, L...>
            { using type = mp_clear<L1>; };

        template<template<class...> class F, class... L>
        struct mp_transform_impl<F, mp_false, L...>
        {
            using _first = F< typename mp_front_impl<L>::type... >;
            using _rest = mp_transform< F, typename mp_pop_front_impl<L>::type... >;
            using type = mp_push_front<_rest, _first>;
        };



        /* unique id */
        template<class Counter>
        struct mp_unique_counter
        {
            static inline size_t next()
            {
                static size_t value;
                return value++;
            }
        };

        template<class Counter, class... Args>
        struct mp_unique_id
        {
            static inline size_t value()
            {
                static const size_t value = mp_unique_counter<Counter>::next();
                return value;
            }
        };



        /* string */
        template <size_t N>
        struct mp_string_impl<N, tag_reference>
        {
        private:
            const char (&_data)[N + 1];

        public:
            inline std::string str() const
                { return std::string(_data, N); }

            inline constexpr const char* data() const
                { return _data; }

            inline constexpr size_t size() const
                { return N; }

            inline constexpr char operator[](size_t i) const
                { return X_ASSERT(i >= 0 && i < N), _data[i]; }

            constexpr mp_string_impl(const char (&data)[N + 1]) :
                _data((X_ASSERT(data[N] == '\0'), data))
                { }
        };

        template <size_t N>
        struct mp_string_impl<N, tag_array>
        {
        private:
            constexpr static size_t strsize(const char* s)
            {
                size_t ret = 0;
                while (s[ret] != 0)
                    ++ret;
                return ret;
            }

        private:
            char _data[N + 1];

            template <size_t N1, class T1, class T2, size_t... I1, size_t... I2>
            constexpr mp_string_impl(
                const mp_string_impl<    N1, T1>& s1,
                const mp_string_impl<N - N1, T2>& s2,
                mp_index_sequence<I1...>,
                mp_index_sequence<I2...>) :
                    _data { s1[I1]..., s2[I2]..., '\0' }
                    { }

        public:
            inline std::string str() const
                { return std::string(_data, N); }

            inline constexpr const char* data() const
                { return _data; }

            inline constexpr size_t size() const
                { return N; }

            inline constexpr char operator[](size_t i) const
                { return X_ASSERT(i >= 0 && i < N), _data[i]; }

            template<size_t X = N, class Enable = mp_enable_if_c<(X == 1), char>>
            constexpr mp_string_impl(char c) :
                _data { c, '\0' }
                { }

            template <size_t N1, class T1, class T2, mp_enable_if_c<(N1 <= N), bool> = true>
            constexpr mp_string_impl(
                const mp_string_impl<N1, T1>& s1,
                const mp_string_impl<N - N1, T2>& s2) :
                    mp_string_impl(s1, s2, mp_make_index_sequence<N1>(), mp_make_index_sequence<N - N1>())
                { }
        };

        template<class X, class Enable = void>
        struct __int_to_char;

        template<class X>
        struct __int_to_char<X, mp_enable_if_c<X::value >= 0 && X::value <= 9>>
            { static constexpr auto value = mp_array_string<1>('0' + X::value); };

        template<class X>
        struct __int_to_char<X, mp_enable_if_c<X::value >= 10 && X::value <= 16>>
            { static constexpr auto value = mp_array_string<1>('A' + X::value - 10); };

        template<class X, int Base, bool b>
        struct __int_to_string_helper
        {
            using _type = typename X::value_type;
            using _next = mp_const<_type, X::value / static_cast<_type>(Base)>;
            using _this = mp_const<_type, X::value % static_cast<_type>(Base)>;
            static constexpr auto value = mp_string_cat(
                    __int_to_string_helper<_next, Base, _next::value == 0>::value,
                    __int_to_char<_this>::value);
        };

        template<class X, int Base>
        struct __int_to_string_helper<X, Base, true>
            { static constexpr auto value = mp_make_string(""); };

        template<class I, int Base>
        struct mp_int_to_string_impl
        {
            static constexpr auto value = __int_to_string_helper<I, Base, I::value == 0>::value;
        };

        template<class J, template<class...> class L, class... T>
        struct mp_string_type_join_impl<J, L<T...>>
        {
            static constexpr auto value = mp_string_join(J::value, T::value...);
        };

    }
}