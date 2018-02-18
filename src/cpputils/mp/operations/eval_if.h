#pragma once

#include <cpputils/mp/misc/when.h>
#include <cpputils/mp/misc/tag_of.h>
#include <cpputils/mp/misc/default.h>
#include <cpputils/mp/intern/has_value.h>
#include <cpputils/mp/operations/if.fwd.h>
#include <cpputils/mp/operations/eval.h>

namespace utl {
namespace mp {

    namespace __impl
    {
        struct eval_if_t
        {
            template <typename Cond, typename Then, typename Else>
            constexpr auto operator()(Cond&& c, Then&& t, Else&& e) const;
        };
    }

    constexpr __impl::eval_if_t eval_if { };

    namespace __impl
    {
        template <typename T, typename = void>
        struct eval_if_impl
            : eval_if_impl<T, when<true>>
        { };

        template <typename T, bool condition>
        struct eval_if_impl<T, when<condition>>
            : default_
        {
            template <typename ...Args>
            static constexpr auto apply(Args&& ...) = delete;
        };

        template <typename T>
        struct eval_if_impl<T, when<is_arithmetic<T>::value>>
        {
            template <typename Cond, typename Then, typename Else>
            static constexpr auto apply(const Cond& cond, Then&& t, Else&& e)
            {
                return cond ? eval(std::forward<Then>(t))
                            : eval(std::forward<Else>(e));
            }
        };

        template <typename T>
        struct eval_if_impl<T, when<intern::has_value<T>::value>>
        {
            template<typename Then, typename Else>
            static constexpr auto eval_if_helper(c_true_t, Then&& t, Else&& e)
                { return eval(std::forward<Then>(t)); }

            template<typename Then, typename Else>
            static constexpr auto eval_if_helper(c_false_t, Then&& t, Else&& e)
                { return eval(std::forward<Else>(e)); }

            template <typename Cond, typename Then, typename Else>
            static constexpr auto apply(const Cond&, Then&& t, Else&& e)
            {
                constexpr auto cond = value<Cond>();
                constexpr bool truth_value = if_(cond, true, false);
                return eval_if_helper(c_bool_t<truth_value> { }, std::forward<Then>(t), std::forward<Else>(e));
            }
        };

        template <typename Cond, typename Then, typename Else>
        constexpr auto eval_if_t::operator()(Cond&& c, Then&& t, Else&& e) const
        {
            using tag_type     = tag_of<Cond>;
            using eval_if_type = eval_if_impl<tag_type>;
            return eval_if_type::apply(std::forward<Cond>(c), std::forward<Then>(t), std::forward<Else>(e));
        }
    }

}
}