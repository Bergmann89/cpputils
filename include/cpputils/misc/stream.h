#pragma once

#include <iostream>
#include "exception.h"

namespace utl
{

    namespace __impl
    {
        template<class T, class Enable = void>
        struct op_write_stream
        {
            inline void operator()(std::ostream& os, const T& t)
            {
                if (!os)
                    throw exception("unable to write data to stream: invalid stream");
                os.write(reinterpret_cast<const char*>(&t), sizeof(t));
                if (!os)
                    throw exception("unable to write data to stream: stream error");
            }
        };

        template<class T, class Enable = void>
        struct op_read_stream
        {
            inline void operator()(std::istream& is, T& t)
            {
                if (!is)
                    throw exception("unable to read data from stream: invalid stream");
                if (is.read(reinterpret_cast<char*>(&t), sizeof(t)).gcount() != sizeof(t))
                    throw exception("unable to read data from stream: EOF");
                if (!is)
                    throw exception("unable to read data from stream: stream error");
            }
        };

        template<>
        struct op_write_stream<std::string, void>
        {
            inline void operator()(std::ostream& os, const std::string& t)
            {
                if (t.size() > std::numeric_limits<uint32_t>::max())
                    throw exception("unable to write data to stream: string is to large");
                op_write_stream<uint32_t, void>()(os, static_cast<uint32_t>(t.size()));
                if (!os)
                    throw exception("unable to write data to stream: invalid stream");
                os.write(t.data(), static_cast<std::streamsize>(t.size()));
                if (!os)
                    throw exception("unable to write data to stream: stream error");
            }
        };

        template<>
        struct op_read_stream<std::string, void>
        {
            inline void operator()(std::istream& is, std::string& t)
            {
                uint32_t sz;
                op_read_stream<uint32_t, void>()(is, sz);
                if (!is)
                    throw exception("unable to read data from stream: invalid stream");
                std::string tmp;
                tmp.resize(sz);
                if (is.read(const_cast<char*>(tmp.data()), sz).gcount() != sz)
                    throw exception("unable to read data from stream: EOF");
                if (!is)
                    throw exception("unable to read data from stream: stream error");
                t = std::move(tmp);
            }
        };

        template<class T>
        struct op_write_stream<T, decltype(std::declval<T>().serialize(std::declval<std::ostream&>()), void())>
        {
            inline void operator()(std::ostream& os, const T& t)
                { t.serialize(os); }
        };

        template<class T>
        struct op_read_stream<T, decltype(std::declval<T>().deserialize(std::declval<std::istream&>()), void())>
        {
            inline void operator()(std::istream& os, T& t)
                { t.deserialize(os); }
        };
    }

    struct stream_helper
    {
        template<class T>
        static inline void write(std::ostream& os, const T& t)
            { __impl::op_write_stream<T>()(os, t); }

        template<class T>
        static inline void read(std::istream& is, T& t)
            { __impl::op_read_stream<T>()(is, t); }

        template<class T>
        static inline T read(std::istream& is)
        {
            T t;
            read<T>(is, t);
            return std::move(t);
        }
    };

    struct stream_format_saver
    {
        std::ios        state;
        std::ostream&   stream;

        stream_format_saver(std::ostream& s) :
            state   (nullptr),
            stream  (s)
            { state.copyfmt(stream); }

        ~stream_format_saver()
            { stream.copyfmt(state); }
    };

}