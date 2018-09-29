#pragma once

#include <atomic>
#include <exception>

namespace utl
{

    struct cancellation_exception
        : public std::exception
    {
        const char* what() const throw() override
            { return "task was canceled!"; }
    };

    template<typename T>
    struct data_cancellation_exception
        : public cancellation_exception
    {
        T data;

        data_cancellation_exception(const T& d)
            : data(d)
            { }
    };

    template<>
    struct data_cancellation_exception<void>
        : public cancellation_exception
        { };

    struct cancellation_token
    {
    private:
        std::atomic<bool> _is_cancellation_requested;

    public:
        virtual void throw_if_cancellation_requested() const = 0;

        inline bool is_cancellation_requested() const
            { return _is_cancellation_requested; }

        inline void cancel()
            { _is_cancellation_requested = true; }

        inline cancellation_token()
            : _is_cancellation_requested(false)
            { }

        virtual ~cancellation_token()
            { }

    private:
        cancellation_token(cancellation_token&&) = delete;
        cancellation_token(const cancellation_token&) = delete;
    };


    template<typename T>
    struct cancellation_source
        : public cancellation_token
    {
        T value;

        void throw_if_cancellation_requested() const override
        {
            if (is_cancellation_requested())
                throw data_cancellation_exception<T>(value);
        }

        void cancel(const T& p_value)
        {
            cancellation_token::cancel();
            value = p_value;
        }
    };

    template<>
    struct cancellation_source<void>
        : public cancellation_token
    {
    public:
        void throw_if_cancellation_requested() const override
        {
            if (is_cancellation_requested())
                throw cancellation_exception();
        }

        inline void cancel()
            { cancellation_token::cancel(); }
    };

}