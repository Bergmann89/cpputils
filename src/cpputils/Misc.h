#pragma once

#include <cxxabi.h>
#include <iostream>

#if defined(__linux__)
#  include <endian.h>
#elif defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/endian.h>
#elif defined(__OpenBSD__)
#  include <sys/types.h>
#  define be16toh(x) betoh16(x)
#  define be32toh(x) betoh32(x)
#  define be64toh(x) betoh64(x)
#endif

namespace utl
{

    /* TypeHelper ********************************************************************************/
    template<class T>
    struct TypeHelper
    {
    public:
        static inline std::string name()
        {
            int status;
            auto name = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
            return std::string(name ? name : typeid(T).name());
        }
    };

    /* simple class that stores a value of type T */
    template<class T>
    struct wrapper
    {
        using value_type = T;

        value_type value;

        inline value_type& operator*()
            { return value; }

        inline const value_type& operator*() const
            { return value; }

        inline wrapper& operator=(const value_type v)
        {
            value = v;
            return *this;
        }

        inline wrapper& operator=(const wrapper& other)
        {
            value = other.value;
            return *this;
        }

        inline wrapper& operator=(wrapper&& other)
        {
            value = std::move(other).value;
            return *this;
        }

        inline wrapper() :
            value(value_type())
            { }

        inline wrapper(const value_type& v) :
            value(v)
            { }

        inline wrapper(const wrapper& other) :
            value(other.value)
            { }

        inline wrapper(wrapper&& other) :
            value(std::move(other.value))
            { }
    };

    template<class T>
    struct wrapper<T&>
    {
        using value_type   = T&;
        using storage_type = T*;

        storage_type value;

        inline value_type operator*() const
        {
            assert(value != nullptr);
            return *value;
        }

        inline wrapper& operator=(const value_type v)
        {
            value = &v;
            return *this;
        }

        inline wrapper& operator=(const wrapper& other)
        {
            value = other.value;
            return *this;
        }

        inline wrapper& operator=(wrapper&& other)
        {
            value = std::move(other.value);
            return *this;
        }

        inline wrapper() :
            value(nullptr)
            { }

        inline wrapper(value_type v) :
            value(&v)
            { }

        inline wrapper(const wrapper& other) :
            value(other.value)
            { }

        inline wrapper(wrapper&& other) :
            value(std::move(other.value))
            { }
    };

    /* Helper Methods ****************************************************************************/
    inline int bitCount(uint32_t u)
    {
        u = u
            - ((u >> 1) & 033333333333)
            - ((u >> 2) & 011111111111);
        return ((u + (u >> 3)) & 030707070707) % 63;
    }

    template<class T, class S>
    inline bool tryCast(T* t, S*& s)
    {
        s = dynamic_cast<S*>(t);
        return static_cast<bool>(s);
    }

    namespace __impl
    {
        template<class T, size_t N = sizeof(T)>
        struct network_convert_helper;
    }

    template<class T>
    inline T hton(const T& value)
        { return __impl::network_convert_helper<T>::hton(value); }

    template<class T>
    inline T ntoh(const T& value)
        { return __impl::network_convert_helper<T>::ntoh(value); }


    /* Indent Stream *****************************************************************************/
    namespace __impl
    {
        inline int identStreamIndex()
        {
            const int value = std::ios::xalloc();
            return value;
        }
    }

    inline std::ostream& incindent(std::ostream& os)
    {
        ++os.iword(__impl::identStreamIndex());
        return os;
    }

    inline std::ostream& decindent(std::ostream& os)
    {
        auto& indent = os.iword(__impl::identStreamIndex());
        if (--indent < 0)
            indent = 0;
        return os;
    }

    inline std::ostream& indent(std::ostream& os)
    {
        auto i = os.iword(__impl::identStreamIndex());
        while (i--)
            os << "  ";
        return os;
    }

    /* implementation ****************************************************************************/
    namespace __impl
    {
        template<class T>
        struct network_convert_helper<T, 1>
        {
            static inline T hton(const T& t)
                { return t; }

            static inline T ntoh(const T& t)
                { return t; }
        };

        template<class T>
        struct network_convert_helper<T, 2>
        {
            static inline T hton(const T& t)
                { return reinterpret_cast<T>(htobe16(reinterpret_cast<uint16_t>(t))); }

            static inline T ntoh(const T& t)
                { return reinterpret_cast<T>(be16toh(reinterpret_cast<uint16_t>(t))); }
        };

        template<class T>
        struct network_convert_helper<T, 4>
        {
            static inline T hton(const T& t)
                { return reinterpret_cast<T>(htobe32(reinterpret_cast<uint32_t>(t))); }

            static inline T ntoh(const T& t)
                { return reinterpret_cast<T>(be32toh(reinterpret_cast<uint32_t>(t))); }
        };

        template<class T>
        struct network_convert_helper<T, 8>
        {
            static inline T hton(const T& t)
                { return reinterpret_cast<T>(htobe64(reinterpret_cast<uint64_t>(t))); }

            static inline T ntoh(const T& t)
                { return reinterpret_cast<T>(be64toh(reinterpret_cast<uint64_t>(t))); }
        };
    }
}