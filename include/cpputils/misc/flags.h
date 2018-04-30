#pragma once

#include <limits>
#include <cassert>
#include <type_traits>
#include <initializer_list>

namespace utl
{
    template<class T_enum, class T_base>
    struct op_flag_convert_none
    {
        static inline T_base to_base(const T_enum& e)
            { return static_cast<T_base>(e); }

        static inline T_enum from_index(const ssize_t& index)
            { return static_cast<T_enum>(1 << index); }
    };

    template<class T_enum, class T_base>
    struct op_flag_convert_shift
    {
        static inline T_base to_base(const T_enum& e)
            { return static_cast<T_base>(1 << static_cast<int>(e)); }

        static inline T_enum from_index(const ssize_t& index)
            { return static_cast<T_enum>(index); }
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
        struct iterator
        {
        private:
            static constexpr ssize_t bit_count = static_cast<ssize_t>(8 * sizeof(base_type));

            base_type   _base;
            ssize_t     _pos;

            inline void next()
            {
                if (_pos >= bit_count)
                    return;
                do
                {
                    ++_pos;
                } while(_pos < bit_count && !static_cast<bool>(_base & (1 << _pos)));
            }

        public:
            iterator(const base_type& base, bool is_end)
                : _base(base)
                , _pos (is_end ? bit_count : -1)
                { next(); }

            inline bool operator == (const iterator& other) const
                { return _base == other._base && _pos == other._pos; }

            inline bool operator != (const iterator& other) const
                { return _base != other._base || _pos != other._pos; }

            inline enum_type operator * () const
            {
                assert(_pos < bit_count);
                return op_type::from_index(_pos);
            }

            inline iterator& operator ++ ()
            {
                next();
                return *this;
            }

            inline iterator operator ++ (int)
            {
                auto ret = *this;
                next();
                return ret;
            }
        };

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

        inline auto begin() const
            { return iterator(value, false); }

        inline auto end() const
            { return iterator(value, true); }

    public:
        inline base_type operator()() const
            { return value; }

        inline operator base_type() const
            { return static_cast<base_type>(value); }

        inline explicit operator bool() const
            { return static_cast<bool>(value); }

        inline bool operator[](enum_type e) const
            { return is_set(e); }

    public:
        inline flags() :
            value(0)
            { }

        inline explicit flags(base_type v) :
            value(v)
            { }

        inline flags(enum_type e) :
            value(T_op::to_base(e))
            { }

        inline flags(const flags& other) :
            value(other.value)
            { }

        inline flags(std::initializer_list<enum_type> list) :
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