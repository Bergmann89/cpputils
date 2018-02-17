#pragma once

#include <string>
#include <cstdint>
#include <iomanip>
#include <string.h>
#include <execinfo.h>

#include "flags.h"

namespace utl
{

    struct exception : public std::exception
    {
    public:
        enum class print_flag
        {
            ResolveAddress,
        };
        using print_flags = shifted_flags<print_flag>;

    public:
        static constexpr size_t max_stack_size = 15;
        static const print_flags& default_print_flag()
        {
            static const print_flags value({ print_flag::ResolveAddress });
            return value;
        };

    private:
        mutable bool        _msg_cache_empty;
        mutable std::string _msg_cache;

    protected:
        virtual void print_message(std::ostream& os) const
        {
            os << message;
        }

    public:
        std::string message;
        void*       stack[max_stack_size];
        int         stack_size;

    public:
        void print(std::ostream& os, const print_flags& flags = default_print_flag()) const
        {
            print_message(os);
            os << std::endl;
            char** lines = nullptr;
            if (flags.is_set(print_flag::ResolveAddress))
                lines = backtrace_symbols(stack, stack_size);
            for (int i = 0; i < stack_size; ++i)
            {
                os << "  [0x" << std::setw(2 * sizeof(void*)) << std::setfill('0') << std::hex << reinterpret_cast<uintptr_t>(stack[i]) << "]";
                if (lines && lines[i])
                    os << " " << lines[i];
                os << std::endl;
            }
        }

        std::string print(const print_flags& flags = default_print_flag()) const
        {
            std::ostringstream os;
            print(os, flags);
            return os.str();
        }

        const char* what() const throw() override
        {
            if (_msg_cache_empty)
            {
                _msg_cache      = print();
                _msg_cache_empty = false;
            }
            return _msg_cache.c_str();
        }

        inline friend std::ostream& operator <<(std::ostream& os, const exception& ex)
        {
            ex.print(os);
            return os;
        }

    public:
        exception() :
            exception("")
            { }

        exception(std::string msg) :
            message         (msg),
            stack_size      (0),
            _msg_cache_empty(true)
        {
            stack_size = backtrace(&stack[0], max_stack_size);
        }
    };

    struct error_exception : public exception
    {
    public:
        int error;

        error_exception(int e) :
            exception(std::to_string(e) + " - " + strerror(e)),
            error(e)
            { }

        error_exception(const std::string& msg, int e) :
            exception(msg + ": " + std::to_string(e) + " - " + strerror(e)),
            error(e)
            { }
    };

    struct out_of_range_exception : public exception
    {
    protected:
        void print_message(std::ostream& os) const override
        {
            os << "index out of range (min=" << min << "; max=" << max << "; index=" << index << ")";
            if (!message.empty())
                os << " - " << message;
        }

    public:
        size_t min;
        size_t max;
        size_t index;

        out_of_range_exception(size_t mi, size_t ma, size_t idx, std::string msg = "") :
            exception   (msg),
            min         (mi),
            max         (ma),
            index       (idx)
            { }
    };

    struct argument_exception : public exception
    {
    protected:
        void print_message(std::ostream& os) const override
        {
            os << "invalid argument";
            if (!argument.empty())
                os << "(" << argument << ")";
            if (!message.empty())
                os << " - " << message;
        }

    public:
        std::string argument;

        argument_exception(std::string arg, std::string msg) :
            exception   (msg),
            argument    (arg)
            { }
    };

    struct invalid_operation_exception : public exception
    {
    public:
        invalid_operation_exception(std::string msg) :
            exception(msg)
            { }
    };

}