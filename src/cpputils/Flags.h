#pragma once

#include <limits>
#include <type_traits>
#include <initializer_list>

namespace utl
{
    template<class TEnum, class TBase>
    struct op_flag_convert_none
    {
        static inline TBase toBase(const TEnum e)
            { return static_cast<TBase>(e); }
    };

    template<class TEnum, class TBase>
    struct op_flag_convert_shift
    {
        static inline TBase toBase(const TEnum e)
            { return static_cast<TBase>(1 << static_cast<int>(e)); }
    };

    template<
        class TEnum,
        class TBase = typename std::underlying_type<TEnum>::type,
        class Op    = op_flag_convert_none<TEnum, TBase>>
    struct Flags
    {
    public:
        TBase value;

    public:
        inline bool isSet(TEnum e) const
            { return static_cast<bool>(value & Op::toBase(e)); }

        inline void set(TEnum e)
            { value = static_cast<TBase>(value | Op::toBase(e)); }

        inline void clear(TEnum e)
            { value = static_cast<TBase>(value & ~Op::toBase(e)); }

        inline void reset()
            { value = 0; }

    public:
        TBase operator()() const
            { return value; }

        operator bool() const
            { return static_cast<bool>(value); }

        bool operator[](TEnum e) const
            { return isSet(e); }

    public:
        Flags() :
            value(0)
            { }

        explicit Flags(TBase v) :
            value(v)
            { }

        Flags(TEnum e) :
            value(Op::toBase(e))
            { }

        Flags(const Flags& other) :
            value(other.value)
            { }

        Flags(std::initializer_list<TEnum> list) :
            Flags()
        {
            for (auto& e : list)
                set(e);
        }

    public:
        static inline const Flags& empty()
        {
            static const Flags value(0);
            return value;
        }

        static inline const Flags& all()
        {
            static const Flags value(std::numeric_limits<TBase>::max());
            return value;
        }
    };

    template<
        class TEnum,
        class TBase = typename std::underlying_type<TEnum>::type>
    using ShiftedFlags = Flags<TEnum, TBase, op_flag_convert_shift<TEnum, TBase>>;

    template<
        class TEnum,
        class TBase = typename std::underlying_type<TEnum>::type>
    using SimpleFlags = Flags<TEnum, TBase, op_flag_convert_none<TEnum, TBase>>;

}