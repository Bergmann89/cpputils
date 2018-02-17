#pragma once

#include <iostream>

namespace utl
{

    namespace __impl
    {
        inline int indent_stream_index()
        {
            const int value = std::ios::xalloc();
            return value;
        }
    }

    inline std::ostream& incindent(std::ostream& os)
    {
        ++os.iword(__impl::indent_stream_index());
        return os;
    }

    inline std::ostream& decindent(std::ostream& os)
    {
        auto& indent = os.iword(__impl::indent_stream_index());
        if (--indent < 0)
            indent = 0;
        return os;
    }

    inline std::ostream& indent(std::ostream& os)
    {
        auto i = os.iword(__impl::indent_stream_index());
        while (i--)
            os << "  ";
        return os;
    }

}