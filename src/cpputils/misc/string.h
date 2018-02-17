#pragma once

#include <sstream>
#include <cpputils/misc/exception.h>

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
        struct op_to_string<T, decltype(std::declval<T>().to_string(std::declval<std::ostream&>()), void())>
        {
            inline void operator()(std::ostream& os, const T& s) const
                { s.to_string(os); }
        };

        template<class T>
        struct op_to_string<T, decltype(std::declval<T>().to_string(), void())>
        {
            inline void operator()(std::ostream& os, const T& s) const
                { os << s.to_string(); }
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
        struct op_from_string<T, decltype(std::declval<T&>().from_string(std::declval<const std::string&>()), void())>
        {
            inline bool operator()(const std::string& s, T& value) const
                { return value.from_string(s); }
        };

        template<class T>
        struct op_from_string<T, decltype(T::from_string(std::declval<const std::string&>(), std::declval<T&>()), void())>
        {
            inline bool operator()(const std::string& s, T& value) const
                { return T::from_string(s, value); }
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
    inline void to_string(std::ostream& os, const T& t)
        { __impl::op_to_string<T>()(os, t); }

    template<class T>
    inline std::string to_string(const T& t)
    {
        std::ostringstream ss;
        to_string(ss, t);
        return ss.str();
    }

    template<>
    inline std::string to_string<std::string>(const std::string& s)
        { return s; }



    template<class T>
    inline bool try_from_string(const std::string& s, T& value)
        { return __impl::op_from_string<T>()(s, value); }

    template<class T>
    inline T from_string(const std::string& s, T defaultValue)
    {
        T tmp;
        return try_from_string<T>(s, tmp)
            ? tmp
            : defaultValue;
    }

    template<class T>
    inline T from_string(const std::string& s)
    {
        T tmp;
        if (!try_from_string<T>(s, tmp))
            throw exception(std::string("unable to convert string to specific value: ") + s);
        return tmp;
    }

}