#pragma once

#include <memory>
#include <cpputils/misc/exception.h>
#include <cpputils/mp/core.h>

namespace utl
{

    template<class T>
    struct smart_ptr
    {
    private:
        template<class Tt_base, class Tt_derived>
        using cop_is_base_of = std::is_base_of<mp::clean_type<Tt_base>, mp::clean_type<Tt_derived>>;

        template<class X, class Enable = void>
        struct __impl_cop_is_value :
            public mp::c_true { };

        template<class X>
        struct __impl_cop_is_value<X*, void> :
            public mp::c_false { };

        template<template<class> class F, class X>
        struct __impl_cop_is_value<F<X>, mp::enable_if<cop_is_base_of<smart_ptr<X>, F<X>>>> :
            public mp::c_false { };

        template<class X>
        struct __impl_cop_is_value<std::reference_wrapper<X>, void> :
            public mp::c_false { };

        template<class X>
        using cop_is_value = __impl_cop_is_value<mp::clean_type<X>>;

        struct op_deleter_noop
            { inline void operator()(T*) { } };

    private:
        std::shared_ptr<T> _value;

    public:
        inline bool has_value() const
            { return static_cast<bool>(_value); }

        inline T& value()
        {
            if (!_value)
                throw utl::invalid_operation_exception("smart_ptr does not have a value");
            return *_value;
        }

        inline const T& value() const
        {
            if (!_value)
                throw utl::invalid_operation_exception("smart_ptr does not have a value");
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
            { return has_value(); }

        inline smart_ptr& reset()
        {
            _value.reset();
            return *this;
        }

        template<class X = T, mp::enable_if_c<cop_is_value<X>::value, int> = 0>
        inline smart_ptr& reset(X& x)
        {
            _value.reset(new X(x));
            return *this;
        }

        template<class X = T, mp::enable_if_c<cop_is_value<X>::value, int> = 0>
        inline smart_ptr& reset(X&& x)
        {
            _value.reset(new X(std::move(x)));
            return *this;
        }

        template<class X = T, mp::enable_if_c<cop_is_base_of<T, X>::value, int> = 0>
        inline smart_ptr& reset(const smart_ptr<X>& other)
        {
            _value = other._value;
            return *this;
        }

        template<class X = T, mp::enable_if_c<cop_is_base_of<T, X>::value, int> = 0>
        inline smart_ptr& reset(smart_ptr<X>&& other)
        {
            _value = std::move(other._value);
            return *this;
        }

        template<class X = T, mp::enable_if_c<cop_is_base_of<T, X>::value, int> = 0>
        inline smart_ptr& reset(std::reference_wrapper<X>&& ref)
        {
            _value.reset(&ref.get(), op_deleter_noop());
            return *this;
        }

        template<class... Args>
        inline smart_ptr& reset(Args&&... args)
        {
            _value.reset(new T(std::forward<Args>(args)...));
            return *this;
        }

        template<class X = T>
        inline smart_ptr& operator=(X&& x)
            { return reset(std::forward<X>(x)); }

        smart_ptr()
            { }

        template<class X = T, mp::enable_if_c<cop_is_value<X>::value, int> = 0>
        smart_ptr(X& x) :
            _value(new X(x))
            { }

        template<class X = T, mp::enable_if_c<cop_is_value<X>::value, int> = 0>
        smart_ptr(X&& x) :
            _value(new X(std::move(x)))
            { }

        template<class X = T, mp::enable_if_c<std::is_base_of<T, X>::value, int> = 0>
        smart_ptr(X* x) :
            _value(x)
            { }

        template<class X = T, mp::enable_if_c<std::is_base_of<T, X>::value, int> = 0>
        smart_ptr(const smart_ptr<X>& other) :
            _value(other._value)
            { }

        template<class X = T, mp::enable_if_c<std::is_base_of<T, X>::value, int> = 0>
        smart_ptr(smart_ptr<X>&& other) :
            _value(std::move(other._value))
            { }

        template<class X = T, mp::enable_if_c<std::is_base_of<T, X>::value, int> = 0>
        smart_ptr(std::reference_wrapper<X>&& ref) :
            _value(&ref.get(), op_deleter_noop())
            { }

        template<class... Args>
        smart_ptr(Args... args) :
            _value(new T(args...))
            { }
    };

}