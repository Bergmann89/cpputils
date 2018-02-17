#pragma once

#include <map>
#include <type_traits>

#include <cpputils/misc/string.h>
#include <cpputils/misc/exception.h>

#define DEFINE_ENUM_TO_STRING_MAP(enum, ...) \
    namespace utl { \
    namespace __impl { \
    template<> \
    struct map_enum_to_string<enum> \
    { \
        using enum_to_string_map_type = std::map<enum, std::string>; \
        static inline enum_to_string_map_type value() \
        { \
            static const enum_to_string_map_type map({ \
                __VA_ARGS__ \
            }); \
            return map; \
        } \
    }; } }

#define DEFINE_STRING_TO_ENUM_MAP(enum, less, ...) \
    namespace utl { \
    namespace __impl { \
    template<> \
    struct map_string_to_enum<enum> \
    { \
        using string_to_enum_map_type = std::map<std::string, enum, less>; \
        static inline string_to_enum_map_type value() \
        { \
            static const string_to_enum_map_type map({ \
                __VA_ARGS__ \
            }); \
            return map; \
        } \
    }; } }

#define DEFINE_DEFAULT_ENUM_VALUE(enum, value) \
    namespace utl { \
    namespace __impl { \
    template<> \
    struct default_enum_value<enum> \
    { \
        static inline enum value() \
            { return value; } \
    }; } }

namespace utl
{

    namespace __impl
    {
        struct invariant_string_less
        {
            inline bool operator()(const std::string& lhs, const std::string& rhs) const
            {
                auto c1 = lhs.c_str();
                auto c2 = rhs.c_str();
                auto l1 = lhs.size();
                auto l2 = rhs.size();
                while (l1 > 0 && l2 > 0 && std::tolower(*c1) == std::tolower(*c2))
                {
                    ++c1;
                    ++c2;
                    --l1;
                    --l2;
                }
                return l1 > 0 && l2 > 0
                    ? std::tolower(*c1) < std::tolower(*c2)
                    : l1 < l2;
            }
        };

        template<class T>
        struct map_enum_to_string
        {
            using enum_to_string_map_type = std::map<T, std::string>;
            static inline enum_to_string_map_type value()
            {
                static const enum_to_string_map_type map;
                return map;
            }
        };

        template<class T>
        struct map_string_to_enum
        {
            using string_to_enum_map_type = std::map<std::string, T>;
            static inline string_to_enum_map_type value()
            {
                static const string_to_enum_map_type map;
                return map;
            }
        };

        template<class T>
        struct default_enum_value
        {
            static inline T value()
                { throw exception("unable to convert string to enum"); }
        };
    }

    using invariant_string_less = __impl::invariant_string_less;

    template<class T>
    struct enum_conversion
    {
        using map_enum_to_string_type = __impl::map_enum_to_string<T>;
        using map_string_to_enum_type = __impl::map_string_to_enum<T>;
        using default_enum_value_type = __impl::default_enum_value<T>;
        using base_type               = typename std::underlying_type<T>::type;

        static inline std::string to_string(T value, bool addValue)
        {
            static const auto& map = map_enum_to_string_type::value();
            auto it = map.find(value);
            std::string ret;
            if (it != map.end())
            {
                ret = it->second;
                if (addValue)
                    ret += '(' + std::to_string(static_cast<base_type>(value)) + ')';
            }
            else
                ret = std::to_string(static_cast<base_type>(value));
            return ret;
        }

        static inline bool try_to_enum(const std::string& str, T& value, bool acceptNumeric)
        {
            static const auto& map = map_string_to_enum_type::value();
            auto it = map.find(str);
            if (it != map.end())
            {
                value = it->second;
                return true;
            }
            if (!acceptNumeric)
                return false;
            char *e = nullptr;
            const char *c = str.c_str();
            value = static_cast<T>(std::strtoull(c, &e, 0));
            return (c != e);
        }

        static inline T to_enum(const std::string& str)
        {
            T value;
            return try_to_enum(str, value)
                ? value
                : default_enum_value_type::value();
        }
    };

    namespace __impl
    {
        template<class T>
        struct op_to_string<T, typename std::enable_if<std::is_enum<T>::value>::type>
        {
            inline void operator()(std::ostream& os, const T& s) const
                { os << enum_conversion<T>::to_string(s, true); }
        };

        template<class T>
        struct op_from_string<T, typename std::enable_if<std::is_enum<T>::value>::type>
        {
            inline bool operator()(const std::string& s, T& value) const
                { return enum_conversion<T>::try_to_enum(s, value, true); }
        };
    }

}