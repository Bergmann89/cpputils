#pragma once

#include <iostream>

namespace utl
{

    namespace __impl
    {
        constexpr long default_indent = 4;

        inline int indent_stream_index()
        {
            static const int value = std::ios::xalloc();
            return value;
        }

        inline int indent_count_stream_index()
        {
            static const int value = std::ios::xalloc();
            return value;
        }

        struct set_indent_impl
            { int value; };
    }

    inline auto setindent(int value)
        { return __impl::set_indent_impl { value }; }

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
        auto c = __impl::default_indent + os.iword(__impl::indent_count_stream_index());
        i *= c;
        if (i >= 0)
        {
            os << std::endl;
            while (i--)
                os.put(' ');
        }
        return os;
    }

}

namespace std
{
    template<typename T_char, typename T_traits>
    inline basic_ostream<T_char, T_traits>& operator<< (basic_ostream<T_char, T_traits>& os, const utl::__impl::set_indent_impl& i)
    {
        using namespace ::utl;
        os.iword(__impl::indent_count_stream_index()) = i.value - __impl::default_indent;
        return os;
    }
}