#pragma once

#include <cpputils/mp/operations/if.fwd.h>

#include <cpputils/mp/util/when.h>
#include <cpputils/mp/util/tag_of.h>
#include <cpputils/mp/util/default.h>
#include <cpputils/mp/operations/eval_if.h>

namespace utl {
namespace mp {

    namespace __impl
    {
        template <typename T>
        struct hold {
            T value;
            constexpr T&& operator()() &&
                { return static_cast<T&&>(value); }
        };

        template <typename L, typename = void>
        struct if_impl
            : if_impl<L, when<true>>
        { };

        template <typename L, bool condition>
        struct if_impl<L, when<condition>>
            : default_
        {
            template <typename Cond, typename Then, typename Else>
            static constexpr auto apply(Cond&& c, Then&& t, Else&& e)
            {
                return eval_if(
                    std::forward<Cond>(c),
                    hold<Then&&> { static_cast<Then&&>(t) },
                    hold<Else&&> { static_cast<Else&&>(e) }
                );
            }
        };

        template <typename Cond, typename Then, typename Else>
        constexpr auto if_t::operator()(Cond&& c, Then&& t, Else&& e) const
        {
            using tag_type = tag_of<Cond>;
            using if_type  = if_impl<tag_type>;
            return if_type::apply(std::forward<Cond>(c), std::forward<Then>(t), std::forward<Else>(e));
        }
    }

}
}