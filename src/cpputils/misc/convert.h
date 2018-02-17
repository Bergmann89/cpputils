#pragma once

namespace utl
{

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