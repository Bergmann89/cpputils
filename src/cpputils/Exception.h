#pragma once

#include <string>
#include <cstdint>
#include <iomanip>
#include <string.h>
#include <execinfo.h>

#include "Flags.h"

namespace utl
{

    struct Exception : public std::exception
    {
    public:
        enum class PrintFlag
        {
            ResolveAddress,
        };
        using PrintFlags = ShiftedFlags<PrintFlag>;

    public:
        static constexpr size_t StackSize = 15;
        static const PrintFlags& defaultPrintFlags()
        {
            static const PrintFlags value({ PrintFlag::ResolveAddress });
            return value;
        };

    private:
        mutable bool        _msgCacheEmpty;
        mutable std::string _msgCache;

    protected:
        virtual void printMessage(std::ostream& os) const
        {
            os << message;
        }

    public:
        std::string message;
        void*       stack[StackSize];
        int         stackSize;

    public:
        void print(std::ostream& os, const PrintFlags& flags = defaultPrintFlags()) const
        {
            printMessage(os);
            os << std::endl;
            char** lines = nullptr;
            if (flags.isSet(PrintFlag::ResolveAddress))
                lines = backtrace_symbols(stack, stackSize);
            for (int i = 0; i < stackSize; ++i)
            {
                os << "  [0x" << std::setw(2 * sizeof(void*)) << std::setfill('0') << std::hex << reinterpret_cast<uintptr_t>(stack[i]) << "]";
                if (lines && lines[i])
                    os << " " << lines[i];
                os << std::endl;
            }
        }

        std::string print(const PrintFlags& flags = defaultPrintFlags()) const
        {
            std::ostringstream os;
            print(os, flags);
            return os.str();
        }

        const char* what() const throw() override
        {
            if (_msgCacheEmpty)
            {
                _msgCache      = print();
                _msgCacheEmpty = false;
            }
            return _msgCache.c_str();
        }

        inline friend std::ostream& operator <<(std::ostream& os, const Exception& ex)
        {
            ex.print(os);
            return os;
        }

    public:
        Exception() :
            Exception("")
            { }

        Exception(std::string msg) :
            message         (msg),
            _msgCacheEmpty  (true)
        {
            stackSize = backtrace(stack, StackSize);
        }
    };

    struct ErrorException : public Exception
    {
    public:
        int error;

        ErrorException(int e) :
            Exception(std::to_string(e) + " - " + strerror(e)),
            error(e)
            { }

        ErrorException(const std::string& msg, int e) :
            Exception(msg + ": " + std::to_string(e) + " - " + strerror(e)),
            error(e)
            { }
    };

    struct EOutOfRange : public Exception
    {
    protected:
        void printMessage(std::ostream& os) const override
        {
            os << "index out of range (min=" << min << "; max=" << max << "; index=" << index << ")";
            if (!message.empty())
                os << " - " << message;
        }

    public:
        size_t min;
        size_t max;
        size_t index;

        EOutOfRange(size_t mi, size_t ma, size_t idx, std::string msg = "") :
            Exception   (msg),
            min         (mi),
            max         (ma),
            index       (idx)
            { }
    };

    struct EArgument : public Exception
    {
    protected:
        void printMessage(std::ostream& os) const override
        {
            os << "invalid argument";
            if (!argument.empty())
                os << "(" << argument << ")";
            if (!message.empty())
                os << " - " << message;
        }

    public:
        std::string argument;

        EArgument(std::string arg, std::string msg) :
            Exception   (message),
            argument    (arg)
            { }
    };

    struct EInvalidOperation : public Exception
    {
    public:
        EInvalidOperation(std::string msg) :
            Exception(msg)
            { }
    };

}