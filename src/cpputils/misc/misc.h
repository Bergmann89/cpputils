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

    inline int bit_count(uint32_t u)
    {
        u = u
            - ((u >> 1) & 033333333333)
            - ((u >> 2) & 011111111111);
        return ((u + (u >> 3)) & 030707070707) % 63;
    }

    template<class T, class S>
    inline bool try_cast(T* t, S*& s)
    {
        s = dynamic_cast<S*>(t);
        return static_cast<bool>(s);
    }

}