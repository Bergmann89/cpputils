#pragma once

#include <sstream>
#include "Nullable.h"
#include "EnumConversion.h"

namespace utl
{

    namespace __impl
    {
        template<class T, class Enable = void>
        struct op_to_string
        {
            inline void operator()(std::ostream& os, const T& s) const
                { os << s; }
        };

        template<class T>
        struct op_to_string<T, decltype(std::declval<T>().toString(std::declval<std::ostream&>()), void())>
        {
            inline void operator()(std::ostream& os, const T& s) const
                { s.toString(os); }
        };

        template<class T>
        struct op_to_string<T, decltype(std::declval<T>().toString(), void())>
        {
            inline void operator()(std::ostream& os, const T& s) const
                { os << s.toString(); }
        };

        template<class T>
        struct op_to_string<T, typename std::enable_if<std::is_enum<T>::value>::type>
        {
            inline void operator()(std::ostream& os, const T& s) const
                { os << EnumConversion<T>::toString(s, true); }
        };

        template<class T>
        struct op_to_string<Nullable<T>, void>
        {
            inline void operator()(std::ostream& os, const Nullable<T>& s) const
            {
                if (s.hasValue())
                    op_to_string<T>()(os, s.value());
                else
                    os << "null";
            }
        };


        template<class T, class Enable = void>
        struct op_from_string
        {
            inline bool operator()(const std::string& s, T& value) const
            {
                std::istringstream ss(s);
                ss >> value;
                return static_cast<bool>(ss);
            }
        };

        template<>
        struct op_from_string<std::string, void>
        {
            inline bool operator()(const std::string& s, std::string& value) const
            {
                value = s;
                return true;
            }
        };

        template<>
        struct op_from_string<const char*, void>
        {
            inline bool operator()(const std::string& s, const char*& value) const
            {
                value = s.c_str();
                return true;
            }
        };

        template<class T>
        struct op_from_string<T, decltype(std::declval<T&>().fromString(std::declval<const std::string&>()), void())>
        {
            inline bool operator()(const std::string& s, T& value) const
                { return value.fromString(s); }
        };

        template<class T>
        struct op_from_string<T, decltype(T::fromString(std::declval<const std::string&>(), std::declval<T&>()), void())>
        {
            inline bool operator()(const std::string& s, T& value) const
                { return T::fromString(s, value); }
        };

        template<class T>
        struct op_from_string<T, typename std::enable_if<std::is_enum<T>::value>::type>
        {
            inline bool operator()(const std::string& s, T& value) const
                { return EnumConversion<T>::tryToEnum(s, value, true); }
        };

        template<class T>
        struct op_from_string<T, typename std::enable_if<std::is_integral<T>::value>::type>
        {
            inline bool operator()(const std::string& s, T& value) const
            {
                char *e = nullptr;
                const char *c = s.c_str();
                value = static_cast<T>(std::strtoull(c, &e, 0));
                return (c != e);
            }
        };

        template<class T>
        struct op_from_string<T, typename std::enable_if<std::is_floating_point<T>::value>::type>
        {
            inline bool operator()(const std::string& s, T& value) const
            {
                char *e = nullptr;
                const char *c = s.c_str();
                value = static_cast<T>(std::strtold(c, &e));
                return (c != e);
            }
        };
    }

    template<class T>
    inline void toString(std::ostream& os, const T& t)
        { __impl::op_to_string<T>()(os, t); }

    template<class T>
    inline std::string toString(const T& t)
    {
        std::ostringstream ss;
        toString(ss, t);
        return ss.str();
    }

    template<>
    inline std::string toString<std::string>(const std::string& s)
        { return s; }



    template<class T>
    inline bool tryFromString(const std::string& s, T& value)
        { return __impl::op_from_string<T>()(s, value); }

    template<class T>
    inline T fromString(const std::string& s, T defaultValue)
    {
        T tmp;
        return tryFromString<T>(s, tmp)
            ? tmp
            : defaultValue;
    }

    template<class T>
    inline T fromString(const std::string& s)
    {
        T tmp;
        if (!tryFromString<T>(s, tmp))
            throw Exception(std::string("unable to convert string to specific value: ") + s);
        return tmp;
    }

}