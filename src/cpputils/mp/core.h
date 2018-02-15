#include <cstddef>
#include <utility>

namespace utl {
namespace mp {

    /*********************************************************************************************/
    /* types *************************************************************************************/
    /*********************************************************************************************/
    template<typename T>
    struct tag_const { };

    template<typename T, T t>
    struct integral_constant :
        public std::integral_constant<T, t>
    {
        using tag = tag_const<T>;
    };

    template<typename T, T t>
    constexpr integral_constant<T, t> make_const { };

    template<bool B>
    using c_bool = integral_constant<bool, B>;

    template<size_t S>
    using c_size_t = integral_constant<size_t, S>;

    template<typename...>
    struct list { };

    template<typename... T>
    struct inherit : T...
        { };

    template<typename T, T... Ints>
    struct integer_sequence
        { };

    template<size_t... Ints>
    using index_sequence = integer_sequence<size_t, Ints...>;



    /*********************************************************************************************/
    /* constants *********************************************************************************/
    /*********************************************************************************************/
    using c_true  = c_bool<true>;
    using c_false = c_bool<false>;
    using c_zero  = c_size_t<0>;



    /*********************************************************************************************/
    /* modifier **********************************************************************************/
    /*********************************************************************************************/
    template<typename T>
    using add_pointer = T*;

    template<typename T>
    using add_reference = T&;

    template<typename T>
    using remove_ptr = typename std::remove_pointer<T>::type;

    template<typename T>
    using remove_const = typename std::remove_const<T>::type;

    template<typename T>
    using remove_cv = typename std::remove_cv<T>::type;

    template<typename T>
    using remove_ref = typename std::remove_reference<T>::type;

    template<typename T>
    using clean_type = remove_cv<remove_ptr<remove_ref<T>>>;

    namespace __impl /* forward declaration */
    {
        template<typename T, typename U = remove_ref<T>>
        struct decay_impl;
    }

    template<class T>
    using decay = typename __impl::decay_impl<T>::type;

    namespace __impl /* implementation */
    {
         template <typename T, typename U = remove_ref<T>>
        struct decay
            { using type = remove_cv<U>; };

        template <typename T, typename U>
        struct decay<T, U[]>
            { using type = U*; };

        template <typename T, typename U, std::size_t N>
        struct decay<T, U[N]>
            { using type = U*; };

        template <typename T, typename R, typename ...A>
        struct decay<T, R(A...)>
            { using type = R(*)(A...); };

        template <typename T, typename R, typename ...A>
        struct decay<T, R(A..., ...)>
            { using type = R(*)(A..., ...); };
    }



    /*********************************************************************************************/
    /* conditionals ******************************************************************************/
    /*********************************************************************************************/
    namespace __impl /* forward declaration */
    {
        template<bool C, typename T, typename E>
        struct if_c_impl;

        template<bool C, typename T, template<typename...> typename E, typename... A>
        struct eval_if_c_impl;
    }

    template<typename T, typename S>
    using is_same = c_bool<std::is_same<T, S>::value>;

    template<typename Base, typename Derived>
    using is_base_of = c_bool<std::is_base_of<Base, Derived>::value>;

    template<typename T, typename S = void>
    using enable_if = typename std::enable_if<T::value, S>::type;

    template<bool B, typename S = void>
    using enable_if_c = typename std::enable_if<B, S>::type;

    template<typename B, typename T, typename E>
    using if_t = typename __impl::if_c_impl<B::value != 0, T, E>::type;

    template<bool B, typename T, typename E>
    using if_c = typename __impl::if_c_impl<B, T, E>::type;

    template<typename C, typename T, template<typename...> typename E, typename... A>
    using eval_if = typename __impl::eval_if_c_impl<C::value != 0, T, E, A...>::type;

    template<bool C, typename T, template<typename...> typename E, typename... A>
    using eval_if_c = typename __impl::eval_if_c_impl<C, T, E, A...>::type;

    namespace __impl /* implementation */
    {
        template<typename T, typename F>
        struct if_c_impl<true, T, F>
            { using type = T; };

        template<typename T, typename F>
        struct if_c_impl<false, T, F>
            { using type = F; };

        template<typename T, template<typename...> typename E, typename... A>
        struct eval_if_c_impl<true, T, E, A...>
            { using type = T; };

        template<typename T, template<typename...> typename E, typename... A>
        struct eval_if_c_impl<false, T, E, A...>
            { using type = E<A...>; };
    }

}
}