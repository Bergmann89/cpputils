#pragma once

#include <map>
#include <type_traits>

#include "Exception.h"

#define DefEnumToStringMap(enum, ...) \
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

#define DefStringToEnumMap(enum, less, ...) \
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

#define DefDefaultEnumValue(enum, value) \
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
                { throw Exception("unable to convert string to enum"); }
        };
    }

    using InvariantStringLess = __impl::invariant_string_less;

    template<class T>
    struct EnumConversion
    {
        using map_enum_to_string = __impl::map_enum_to_string<T>;
        using map_string_to_enum = __impl::map_string_to_enum<T>;
        using default_enum_value = __impl::default_enum_value<T>;
        using base_type          = typename std::underlying_type<T>::type;

        static inline std::string toString(T value, bool addValue)
        {
            static const auto& map = map_enum_to_string::value();
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

        static inline bool tryToEnum(const std::string& str, T& value, bool acceptNumeric)
        {
            static const auto& map = map_string_to_enum::value();
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

        static inline T toEnum(const std::string& str)
        {
            T value;
            return tryToEnum(str, value)
                ? value
                : default_enum_value::value();
        }
    };

}