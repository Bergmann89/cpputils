#pragma once

#include <utility>
#include <cpputils/mp/misc/when.h>

namespace utl {
namespace mp {
namespace intern {

    namespace __impl
    {
        template <unsigned int n, typename = when<true>>
        struct fold_left_impl;

        template<>
        struct fold_left_impl<1>
        {
            template<typename F, typename X1>
            static constexpr X1 apply(F&&, X1&& x1)
                { return std::forward<X1>(x1); }
        };

        template<>
        struct fold_left_impl<2>
        {
            template<typename F, typename X1, typename X2>
            static constexpr auto apply(F&& f, X1&& x1, X2&& x2)
            {
                return f(
                    std::forward<X1>(x1),
                    std::forward<X2>(x2));
            }
        };

        template<>
        struct fold_left_impl<3>
        {
            template<typename F, typename X1, typename X2, typename X3>
            static constexpr auto apply(F&& f, X1&& x1, X2&& x2, X3&& x3)
            {
                return f(f(
                    std::forward<X1>(x1),
                    std::forward<X2>(x2)),
                    std::forward<X3>(x3));
            }
        };

        template<>
        struct fold_left_impl<4>
        {
            template<typename F, typename X1, typename X2, typename X3, typename X4>
            static constexpr auto apply(F&& f, X1&& x1, X2&& x2, X3&& x3, X4&& x4)
            {
                return f(f(f(
                    std::forward<X1>(x1),
                    std::forward<X2>(x2)),
                    std::forward<X3>(x3)),
                    std::forward<X4>(x4));
            }
        };

        template<>
        struct fold_left_impl<5>
        {
            template<typename F, typename X1, typename X2, typename X3, typename X4, typename X5>
            static constexpr auto apply(F&& f, X1&& x1, X2&& x2, X3&& x3, X4&& x4, X5&& x5)
            {
                return f(f(f(f(
                    std::forward<X1>(x1),
                    std::forward<X2>(x2)),
                    std::forward<X3>(x3)),
                    std::forward<X4>(x4)),
                    std::forward<X5>(x5));
            }
        };

        template<>
        struct fold_left_impl<6>
        {
            template<typename F, typename X1, typename X2, typename X3, typename X4, typename X5, typename X6>
            static constexpr auto apply(F&& f, X1&& x1, X2&& x2, X3&& x3, X4&& x4, X5&& x5, X6&& x6)
            {
                return f(f(f(f(f(
                    std::forward<X1>(x1),
                    std::forward<X2>(x2)),
                    std::forward<X3>(x3)),
                    std::forward<X4>(x4)),
                    std::forward<X5>(x5)),
                    std::forward<X6>(x6));
            }
        };

        template<>
        struct fold_left_impl<7>
        {
            template<typename F, typename X1, typename X2, typename X3, typename X4, typename X5, typename X6, typename X7>
            static constexpr auto apply(F&& f, X1&& x1, X2&& x2, X3&& x3, X4&& x4, X5&& x5, X6&& x6, X7&& x7)
            {
                return f(f(f(f(f(f(
                    std::forward<X1>(x1),
                    std::forward<X2>(x2)),
                    std::forward<X3>(x3)),
                    std::forward<X4>(x4)),
                    std::forward<X5>(x5)),
                    std::forward<X6>(x6)),
                    std::forward<X7>(x7));
            }
        };

        template<>
        struct fold_left_impl<8>
        {
            template<typename F, typename X1, typename X2, typename X3, typename X4, typename X5, typename X6, typename X7, typename X8>
            static constexpr auto apply(F&& f, X1&& x1, X2&& x2, X3&& x3, X4&& x4, X5&& x5, X6&& x6, X7&& x7, X8&& x8)
            {
                return f(f(f(f(f(f(f(
                    std::forward<X1>(x1),
                    std::forward<X2>(x2)),
                    std::forward<X3>(x3)),
                    std::forward<X4>(x4)),
                    std::forward<X5>(x5)),
                    std::forward<X6>(x6)),
                    std::forward<X7>(x7)),
                    std::forward<X8>(x8));
            }
        };

        template<unsigned int n>
        struct fold_left_impl<n, when<(n >= 9) && (n < 18)>>
        {
            template<typename F, typename X1, typename X2, typename X3, typename X4, typename X5, typename X6, typename X7, typename X8, typename X9, typename... Xn>
            static constexpr auto apply(F&& f, X1&& x1, X2&& x2, X3&& x3, X4&& x4, X5&& x5, X6&& x6, X7&& x7, X8&& x8, X9&& x9, Xn&&... xn)
            {
                return fold_left_impl<sizeof...(xn) + 1>::apply(
                    f,
                    f(f(f(f(f(f(f(f(
                        std::forward<X1>(x1),
                        std::forward<X2>(x2)),
                        std::forward<X3>(x3)),
                        std::forward<X4>(x4)),
                        std::forward<X5>(x5)),
                        std::forward<X6>(x6)),
                        std::forward<X7>(x7)),
                        std::forward<X8>(x8)),
                        std::forward<X9>(x9)),
                    std::forward<Xn>(xn)...
                );
            }
        };

        template<unsigned int n>
        struct fold_left_impl<n, when<(n >= 18) && (n < 36)>>
        {
            template<
                typename F,
                typename X1,  typename X2,  typename X3,  typename X4,  typename X5,  typename X6,  typename X7,  typename X8,  typename X9,
                typename X10, typename X11, typename X12, typename X13, typename X14, typename X15, typename X16, typename X17, typename X18,
                typename... Xn>
            static constexpr auto apply(
                F&& f,
                X1&&  x1,  X2&&  x2,  X3&&  x3,  X4&&  x4,  X5&&  x5,  X6&&  x6,  X7&&  x7,  X8&&  x8,  X9&&  x9,
                X10&& x10, X11&& x11, X12&& x12, X13&& x13, X14&& x14, X15&& x15, X16&& x16, X17&& x17, X18&& x18,
                Xn&&... xn)
            {
                return fold_left_impl<sizeof...(xn) + 1>::apply(
                    f,
                    f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(
                        std::forward<X1> (x1),
                        std::forward<X2> (x2)),
                        std::forward<X3> (x3)),
                        std::forward<X4> (x4)),
                        std::forward<X5> (x5)),
                        std::forward<X6> (x6)),
                        std::forward<X7> (x7)),
                        std::forward<X8> (x8)),
                        std::forward<X9> (x9)),
                        std::forward<X10>(x10)),
                        std::forward<X11>(x11)),
                        std::forward<X12>(x12)),
                        std::forward<X13>(x13)),
                        std::forward<X14>(x14)),
                        std::forward<X15>(x15)),
                        std::forward<X16>(x16)),
                        std::forward<X17>(x17)),
                        std::forward<X18>(x18)),
                    std::forward<Xn>(xn)...
                );
            }
        };

        template<unsigned int n>
        struct fold_left_impl<n, when<(n >= 36)>>
        {
            template<
                typename F,
                typename X1,  typename X2,  typename X3,  typename X4,  typename X5,  typename X6,  typename X7,  typename X8,  typename X9,
                typename X10, typename X11, typename X12, typename X13, typename X14, typename X15, typename X16, typename X17, typename X18,
                typename X19, typename X20, typename X21, typename X22, typename X23, typename X24, typename X25, typename X26, typename X27,
                typename X28, typename X29, typename X30, typename X31, typename X32, typename X33, typename X34, typename X35, typename X36,
                typename... Xn>
            static constexpr auto apply(
                F&& f,
                X1&&  x1,  X2&&  x2,  X3&&  x3,  X4&&  x4,  X5&&  x5,  X6&&  x6,  X7&&  x7,  X8&&  x8,  X9&&  x9,
                X10&& x10, X11&& x11, X12&& x12, X13&& x13, X14&& x14, X15&& x15, X16&& x16, X17&& x17, X18&& x18,
                X19&& x19, X20&& x20, X21&& x21, X22&& x22, X23&& x23, X24&& x24, X25&& x25, X26&& x26, X27&& x27,
                X28&& x28, X29&& x29, X30&& x30, X31&& x31, X32&& x32, X33&& x33, X34&& x34, X35&& x35, X36&& x36,
                Xn&&... xn)
            {
                return fold_left_impl<sizeof...(xn) + 1>::apply(
                    f,
                    f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(
                        std::forward<X1> (x1),
                        std::forward<X2> (x2)),
                        std::forward<X3> (x3)),
                        std::forward<X4> (x4)),
                        std::forward<X5> (x5)),
                        std::forward<X6> (x6)),
                        std::forward<X7> (x7)),
                        std::forward<X8> (x8)),
                        std::forward<X9> (x9)),
                        std::forward<X10>(x10)),
                        std::forward<X11>(x11)),
                        std::forward<X12>(x12)),
                        std::forward<X13>(x13)),
                        std::forward<X14>(x14)),
                        std::forward<X15>(x15)),
                        std::forward<X16>(x16)),
                        std::forward<X17>(x17)),
                        std::forward<X18>(x18)),
                        std::forward<X19>(x19)),
                        std::forward<X20>(x20)),
                        std::forward<X21>(x21)),
                        std::forward<X22>(x22)),
                        std::forward<X23>(x23)),
                        std::forward<X24>(x24)),
                        std::forward<X25>(x25)),
                        std::forward<X26>(x26)),
                        std::forward<X27>(x27)),
                        std::forward<X28>(x28)),
                        std::forward<X29>(x29)),
                        std::forward<X30>(x30)),
                        std::forward<X31>(x31)),
                        std::forward<X32>(x32)),
                        std::forward<X33>(x33)),
                        std::forward<X34>(x34)),
                        std::forward<X35>(x35)),
                        std::forward<X36>(x36)),
                    std::forward<Xn>(xn)...
                );
            }
        };

        struct fold_left_t
        {
            template<typename F, typename X1, typename... Xn>
            constexpr auto operator()(F&& f, X1&& x1, Xn&&... xn) const
                { return fold_left_impl<sizeof...(xn) + 1>::apply(std::forward<F>(f), std::forward<X1>(x1), std::forward<Xn>(xn)...); }
        };
    }

    constexpr __impl::fold_left_t fold_left { };

}
}
}