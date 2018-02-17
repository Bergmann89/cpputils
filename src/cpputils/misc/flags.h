#pragma once

#include <limits>
#include <type_traits>
#include <initializer_list>

namespace utl
{
    template<class TEnum, class TBase>
    struct op_flag_convert_none
    {
        static inline TBase to_base(const TEnum e)
            { return static_cast<TBase>(e); }
    };

    template<class TEnum, class TBase>
    struct op_flag_convert_shift
    {
        static inline TBase to_base(const TEnum e)
            { return static_cast<TBase>(1 << static_cast<int>(e)); }
    };

    template<
        class TEnum,
        class TBase = typename std::underlying_type<TEnum>::type,
        class Op    = op_flag_convert_none<TEnum, TBase>>
    struct flags
    {
    public:
        TBase value;

    public:
        inline bool is_set(TEnum e) const
            { return static_cast<bool>(value & Op::to_base(e)); }

        inline void set(TEnum e)
            { value = static_cast<TBase>(value | Op::to_base(e)); }

        inline void clear(TEnum e)
            { value = static_cast<TBase>(value & ~Op::to_base(e)); }

        inline void reset()
            { value = 0; }

    public:
        TBase operator()() const
            { return value; }

        operator bool() const
            { return static_cast<bool>(value); }

        bool operator[](TEnum e) const
            { return is_set(e); }

    public:
        flags() :
            value(0)
            { }

        explicit flags(TBase v) :
            value(v)
            { }

        flags(TEnum e) :
            value(Op::to_base(e))
            { }

        flags(const flags& other) :
            value(other.value)
            { }

        flags(std::initializer_list<TEnum> list) :
            flags()
        {
            for (auto& e : list)
                set(e);
        }

    public:
        static inline const flags& empty()
        {
            static const flags value(0);
            return value;
        }

        static inline const flags& all()
        {
            static const flags value(std::numeric_limits<TBase>::max());
            return value;
        }
    };

    template<
        class TEnum,
        class TBase = typename std::underlying_type<TEnum>::type>
    using shifted_flags = flags<TEnum, TBase, op_flag_convert_shift<TEnum, TBase>>;

    template<
        class TEnum,
        class TBase = typename std::underlying_type<TEnum>::type>
    using simple_flags = flags<TEnum, TBase, op_flag_convert_none<TEnum, TBase>>;

}