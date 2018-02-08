#pragma once

#include <iostream>

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

    /* Helper Methods ****************************************************************************/
    inline int bitCount(uint32_t u)
    {
        u = u
            - ((u >> 1) & 033333333333)
            - ((u >> 2) & 011111111111);
        return ((u + (u >> 3)) & 030707070707) % 63;
    }

    template<class T, class S>
    bool tryCast(T* t, S*& s)
    {
        s = dynamic_cast<S*>(t);
        return static_cast<bool>(s);
    }

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

}