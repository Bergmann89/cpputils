#pragma once

#include <utility>
#include <cpputils/mp/util/when.h>
#include <cpputils/mp/util/tag_of.h>
#include <cpputils/mp/util/default.h>

namespace utl {
namespace mp {

    namespace __impl
    {
        struct eval_t
        {
            template <typename Expr>
            constexpr auto operator()(Expr&& expr) const;
        };
    }

    constexpr __impl::eval_t eval { };

    namespace __impl
    {
        template <typename T, typename = void>
        struct eval_impl
            : eval_impl<T, when<true>>
        { };

        template <typename T, bool condition>
        struct eval_impl<T, when<condition>>
            : default_
        {
            template <typename Expr>
            static constexpr auto eval_helper(Expr&& expr, int)
                { return static_cast<Expr&&>(expr)(); }

            template <typename Expr>
            static constexpr auto eval_helper(Expr&&, ...)
            {
                static_assert(wrong<Expr> { },
                "eval(expr) requires the expression to be a a nullary Callable");
            }

            template <typename Expr>
            static constexpr decltype(auto) apply(Expr&& expr)
                { return eval_helper(static_cast<Expr&&>(expr), int { }); }
        };

        template <typename Expr>
        constexpr auto eval_t::operator()(Expr&& expr) const
        {
            using tag_type = tag_of<Expr>;
            using eval_impl_type = eval_impl<tag_type>;
            return eval_impl_type::apply(std::forward<Expr>(expr));;
        }
    }

}
}