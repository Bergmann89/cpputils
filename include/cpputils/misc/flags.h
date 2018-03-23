#pragma once

#include <limits>
#include <type_traits>
#include <initializer_list>

namespace utl
{
    template<class T_enum, class T_base>
    struct op_flag_convert_none
    {
        static inline T_base to_base(const T_enum e)
            { return static_cast<T_base>(e); }
    };

    template<class T_enum, class T_base>
    struct op_flag_convert_shift
    {
        static inline T_base to_base(const T_enum e)
            { return static_cast<T_base>(1 << static_cast<int>(e)); }
    };

    template<
        class T_enum,
        class T_base = typename std::underlying_type<T_enum>::type,
        class T_op   = op_flag_convert_none<T_enum, T_base>>
    struct flags
    {
    public:
        using enum_type = T_enum;
        using base_type = T_base;
        using op_type   = T_op;

    public:
        base_type value;

    public:
        inline bool is_set(enum_type e) const
            { return static_cast<bool>(value & op_type::to_base(e)); }

        inline void set(enum_type e)
            { value = static_cast<base_type>(value | op_type::to_base(e)); }

        inline void clear(enum_type e)
            { value = static_cast<base_type>(value & ~op_type::to_base(e)); }

        inline void reset()
            { value = 0; }

    public:
        base_type operator()() const
            { return value; }

        operator base_type() const
            { return static_cast<bool>(value); }

        explicit operator bool() const
            { return static_cast<bool>(value); }

        bool operator[](enum_type e) const
            { return is_set(e); }

    public:
        flags() :
            value(0)
            { }

        explicit flags(base_type v) :
            value(v)
            { }

        flags(enum_type e) :
            value(T_op::to_base(e))
            { }

        flags(const flags& other) :
            value(other.value)
            { }

        flags(std::initializer_list<enum_type> list) :
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
            static const flags value(std::numeric_limits<base_type>::max());
            return value;
        }
    };

    template<
        class T_enum,
        class T_base = typename std::underlying_type<T_enum>::type>
    using shifted_flags = flags<T_enum, T_base, op_flag_convert_shift<T_enum, T_base>>;

    template<
        class T_enum,
        class T_base = typename std::underlying_type<T_enum>::type>
    using simple_flags = flags<T_enum, T_base, op_flag_convert_none<T_enum, T_base>>;

}