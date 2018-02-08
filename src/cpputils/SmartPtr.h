#pragma once

#include <memory>
#include "MetaProgramming.h"
#include "Exception.h"

namespace utl
{

    template<class T>
    struct SmartPtr
    {
    private:
        template<class Tt_base, class Tt_derived>
        using cop_is_base_of = std::is_base_of<utl::mp_clean_type<Tt_base>, utl::mp_clean_type<Tt_derived>>;

        template<class X, class Enable = void>
        struct __impl_cop_is_value :
            public utl::mp_true { };

        template<class X>
        struct __impl_cop_is_value<X*, void> :
            public utl::mp_false { };

        template<template<class> class F, class X>
        struct __impl_cop_is_value<F<X>, utl::mp_enable_if<cop_is_base_of<SmartPtr<X>, F<X>>>> :
            public utl::mp_false { };

        template<class X>
        struct __impl_cop_is_value<std::reference_wrapper<X>, void> :
            public utl::mp_false { };

        template<class X>
        using cop_is_value = __impl_cop_is_value<utl::mp_clean_type<X>>;

        struct op_deleter_noop
            { inline void operator()(T*) { } };

    private:
        std::shared_ptr<T> _value;

    public:
        inline bool hasValue() const
            { return static_cast<bool>(_value); }

        inline T& value()
        {
            if (!_value)
                throw utl::EInvalidOperation("SmartPtr does not have a value");
            return *_value;
        }

        inline const T& value() const
        {
            if (!_value)
                throw utl::EInvalidOperation("SmartPtr does not have a value");
            return *_value;
        }

        inline T& operator*()
            { return value(); }

        inline const T& operator*() const
            { return value(); }

        inline T* operator->()
            { return &value(); }

        inline const T* operator->() const
            { return &value(); }

        inline operator bool() const
            { return hasValue(); }

        inline SmartPtr& reset()
        {
            _value.reset();
            return *this;
        }

        template<class X = T, utl::mp_enable_if_c<cop_is_value<X>::value, int> = 0>
        inline SmartPtr& reset(X& x)
        {
            _value.reset(new X(x));
            return *this;
        }

        template<class X = T, utl::mp_enable_if_c<cop_is_value<X>::value, int> = 0>
        inline SmartPtr& reset(X&& x)
        {
            _value.reset(new X(std::move(x)));
            return *this;
        }

        template<class X = T, utl::mp_enable_if_c<cop_is_base_of<T, X>::value, int> = 0>
        inline SmartPtr& reset(const SmartPtr<X>& other)
        {
            _value = other._value;
            return *this;
        }

        template<class X = T, utl::mp_enable_if_c<cop_is_base_of<T, X>::value, int> = 0>
        inline SmartPtr& reset(SmartPtr<X>&& other)
        {
            _value = std::move(other._value);
            return *this;
        }

        template<class X = T, utl::mp_enable_if_c<cop_is_base_of<T, X>::value, int> = 0>
        inline SmartPtr& reset(std::reference_wrapper<X>&& ref)
        {
            _value.reset(&ref.get(), op_deleter_noop());
            return *this;
        }

        template<class... Args>
        inline SmartPtr& reset(Args&&... args)
        {
            _value.reset(new T(std::forward<Args>(args)...));
            return *this;
        }

        template<class X = T>
        inline SmartPtr& operator=(X&& x)
            { return reset(std::forward<X>(x)); }

        SmartPtr()
            { }

        template<class X = T, utl::mp_enable_if_c<cop_is_value<X>::value, int> = 0>
        SmartPtr(X& x) :
            _value(new X(x))
            { }

        template<class X = T, utl::mp_enable_if_c<cop_is_value<X>::value, int> = 0>
        SmartPtr(X&& x) :
            _value(new X(std::move(x)))
            { }

        template<class X = T, utl::mp_enable_if_c<std::is_base_of<T, X>::value, int> = 0>
        SmartPtr(X* x) :
            _value(x)
            { }

        template<class X = T, utl::mp_enable_if_c<std::is_base_of<T, X>::value, int> = 0>
        SmartPtr(const SmartPtr<X>& other) :
            _value(other._value)
            { }

        template<class X = T, utl::mp_enable_if_c<std::is_base_of<T, X>::value, int> = 0>
        SmartPtr(SmartPtr<X>&& other) :
            _value(std::move(other._value))
            { }

        template<class X = T, utl::mp_enable_if_c<std::is_base_of<T, X>::value, int> = 0>
        SmartPtr(std::reference_wrapper<X>&& ref) :
            _value(&ref.get(), op_deleter_noop())
            { }

        template<class... Args>
        SmartPtr(Args... args) :
            _value(new T(args...))
            { }
    };

}