#include "util.h"

namespace utl {
namespace mp {

    namespace __impl /* forward declaration */
    {
        /* to */
        template<typename To>
        struct to_t;

        template <typename From, typename To, typename = void>
        struct is_convertible_impl;

        template <typename From, typename To, typename = void>
        struct is_embedded_impl;
    }

    /* to */
    template<typename To>
    constexpr __impl::to_t<To> to { };

    template <typename From, typename To>
    using is_convertible = __impl::is_convertible_impl<From, To>;

    template <typename From, typename To>
    using is_embedded = __impl::is_embedded_impl<From, To>;

    namespace __impl /* implementation */
    {
        /* to */
        struct no_conversion { };

        template <bool = true>
        struct embedding { };

        template <typename To, typename From, typename = void>
        struct to_impl;

        template <typename To, typename From, typename>
        struct to_impl :
            public to_impl<To, From, when<true>>
            { };

        template <typename To, typename From, bool condition>
        struct to_impl<To, From, when<condition>> :
            public no_conversion
        {
            template <typename X>
            static constexpr auto apply(const X&)
                { static_assert(wrong<to_impl<To, From>, X> { }, "no conversion is available between the provided types"); }
        };

        template <typename To, typename From>
        struct to_impl<To, From, when_valid<decltype(static_cast<To>(std::declval<From>()))>>
        {
            template <typename X>
            static constexpr To apply(X&& x)
                { return static_cast<To>(std::forward<X>(x)); }
        };

        template <typename To>
        struct to_impl<To, To> :
            public embedding<>
        {
            template <typename X>
            static constexpr X apply(X&& x)
                { return std::forward<X&&>(x); }
        };

        template <typename To>
        struct to_t
        {
            template <typename X>
            constexpr auto operator()(X&& x) const
            {
                using From = tag_of<X>;
                return to_impl<To, From>::apply(std::forward<X>(x));
            }
        };

        #define DEFINE_EMBEDDING_IMPL(TO, FROM)     \
            template <>                             \
            struct to_impl<TO, FROM> :              \
                public embedding<>                  \
            {                                       \
                static constexpr TO apply(FROM x)   \
                    { return static_cast<TO>(x); }  \
            }

        DEFINE_EMBEDDING_IMPL(long double, double);
        DEFINE_EMBEDDING_IMPL(long double, float);
        DEFINE_EMBEDDING_IMPL(double     , float);

        DEFINE_EMBEDDING_IMPL(signed long long, signed long);
        DEFINE_EMBEDDING_IMPL(signed long long, signed int);
        DEFINE_EMBEDDING_IMPL(signed long long, signed short);
        DEFINE_EMBEDDING_IMPL(signed long long, signed char);
        DEFINE_EMBEDDING_IMPL(signed long     , signed int);
        DEFINE_EMBEDDING_IMPL(signed long     , signed short);
        DEFINE_EMBEDDING_IMPL(signed long     , signed char);
        DEFINE_EMBEDDING_IMPL(signed int      , signed short);
        DEFINE_EMBEDDING_IMPL(signed int      , signed char);
        DEFINE_EMBEDDING_IMPL(signed short    , signed char);

        DEFINE_EMBEDDING_IMPL(unsigned long long, unsigned long);
        DEFINE_EMBEDDING_IMPL(unsigned long long, unsigned int);
        DEFINE_EMBEDDING_IMPL(unsigned long long, unsigned short);
        DEFINE_EMBEDDING_IMPL(unsigned long long, unsigned char);
        DEFINE_EMBEDDING_IMPL(unsigned long     , unsigned int);
        DEFINE_EMBEDDING_IMPL(unsigned long     , unsigned short);
        DEFINE_EMBEDDING_IMPL(unsigned long     , unsigned char);
        DEFINE_EMBEDDING_IMPL(unsigned int      , unsigned short);
        DEFINE_EMBEDDING_IMPL(unsigned int      , unsigned char);
        DEFINE_EMBEDDING_IMPL(unsigned short    , unsigned char);

        #undef DEFINE_EMBEDDING_IMPL

        template<class T>
        using copy_char_signedness = if_t<
            std::is_signed<char>,
            typename std::make_signed<T>::type,
            typename std::make_unsigned<T>::type>;

        #define DEFINE_CHAR_EMBEDDING_IMPL(TO)                          \
            template <>                                                 \
            struct to_impl<copy_char_signedness<TO>, char>              \
                : public embedding<>                                    \
            {                                                           \
                static constexpr copy_char_signedness<TO> apply(char x) \
                    { return static_cast<TO>(x); }                      \
            }

        DEFINE_CHAR_EMBEDDING_IMPL(long long);
        DEFINE_CHAR_EMBEDDING_IMPL(long);
        DEFINE_CHAR_EMBEDDING_IMPL(int);
        DEFINE_CHAR_EMBEDDING_IMPL(short);

        #undef DEFINE_CHAR_EMBEDDING_IMPL

        template <typename T>
        struct to_impl<T*, decltype(nullptr)> :
            public embedding<>
        {
            static constexpr T* apply(decltype(nullptr))
                { return nullptr; }
        };

        template <typename From, typename To, typename>
        struct is_convertible_impl :
            public std::true_type
            { };

        template <typename From, typename To>
        struct is_convertible_impl<From, To, decltype((void) static_cast<no_conversion>(std::declval<to_impl<To, From>>()))> :
            public std::false_type
            { };

        template <typename From, typename To, typename>
        struct is_embedded_impl :
            public std::false_type
            { };

        template <typename From, typename To>
        struct is_embedded_impl<From, To, decltype((void) static_cast<embedding<true>>(std::declval<to_impl<To, From>>()))> :
            public std::true_type
            { };
    }

}
}