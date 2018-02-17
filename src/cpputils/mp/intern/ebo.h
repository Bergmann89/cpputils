#pragma once

#include "../core.h"

namespace utl {
namespace mp {
namespace intern {

    template<typename Key, typename Value, bool = is_empty<Value>::value && !is_final<Value>::value>
    struct ebo;

    template<typename Key, typename Value>
    constexpr const Value& ebo_get(const ebo<Key, Value, true>& x)
        { return x; }

    template<typename Key, typename Value>
    constexpr Value& ebo_get(ebo<Key, Value, true>& x)
        { return x; }

    template<typename Key, typename Value>
    constexpr Value&& ebo_get(ebo<Key, Value, true>&& x)
        { return x; }

    template<typename Key, typename Value>
    constexpr const Value& ebo_get(const ebo<Key, Value, false>& x)
        { return x._data; }

    template<typename Key, typename Value>
    constexpr Value& ebo_get(ebo<Key, Value, false>& x)
        { return x._data; }

    template<typename Key, typename Value>
    constexpr Value&& ebo_get(ebo<Key, Value, false>&& x)
        { return static_cast<Value&&>(x._data); }

    template<typename Key, typename Value>
    struct ebo<Key, Value, true> :
        public Value
    {
        constexpr ebo()
            { }

        template<typename T>
        explicit constexpr ebo(T&& t) :
            Value(std::forward<T>(t))
            { }

        constexpr auto get() const&
            { return ebo_get(*this); }

        constexpr auto get() &
            { return ebo_get(*this); }

        constexpr auto get() &&
            { return ebo_get(std::move(*this)); }
    };

    template<typename Key, typename Value>
    struct ebo<Key, Value, false>
    {
        Value _data;

        constexpr ebo()
            { }

        template<typename T>
        explicit constexpr ebo(T&& t) :
            _data(std::forward<T>(t))
            { }

        constexpr auto get() const&
            { return ebo_get(*this); }

        constexpr auto get() &
            { return ebo_get(*this); }

        constexpr auto get() &&
            { return ebo_get(std::move(*this)); }
    };

}
}
}