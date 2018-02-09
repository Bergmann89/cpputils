#pragma once

#include <memory>
#include <type_traits>
#include "Exception.h"

namespace utl
{

    template<class T>
    struct Nullable
    {
    public:
        using value_type    = T;
        using clean_type    = typename std::remove_reference<T>::type;
        using pointer_type  = clean_type*;

    private:
        struct Container
        {
            value_type value;

            template<class... Args>
            Container(Args&&... args) :
                value(std::forward<Args>(args)...)
                { }
        };

    private:
        std::unique_ptr<Container> _container;

        inline void check() const
        {
            if (!_container)
                throw EInvalidOperation("nullable does not have a value");
        }

    public:
        inline value_type& value()
            { check(); return _container->value; }

        inline const value_type& value() const
            { check(); return _container->value; }

        inline bool hasValue() const
            { return static_cast<bool>(_container); }

        inline void reset()
            { _container.reset(); }



        inline auto& operator*()
            { return value(); }

        inline auto& operator*() const
            { return value(); }

        inline auto* operator->()
            { return &value(); }

        inline auto* operator->() const
            { return &value(); }

        inline value_type& operator()()
            { return value(); }

        inline auto& operator()() const
            { return value(); }

        inline explicit operator bool() const
            { return hasValue(); }

        inline bool operator==(T v) const
            { return hasValue() && value() == v; }

        inline bool operator==(const Nullable& other) const
            { return hasValue() && other.hasValue() && value() == other.value(); }

        inline bool operator<(const Nullable& other) const
        {
            return this->hasValue()
                && other.hasValue()
                && value() < other.value();
        }

        inline bool operator>(const Nullable& other) const
        {
            return this->hasValue()
                && other.hasValue()
                && value() < other.value();
        }

        inline bool operator<=(const Nullable& other) const
        {
            return this->hasValue()
                && other.hasValue()
                && value() < other.value();
        }

        inline bool operator>=(const Nullable& other) const
        {
            return this->hasValue()
                && other.hasValue()
                && value() < other.value();
        }

        inline void operator=(value_type&& t)
            { _container.reset(new Container(std::forward<value_type>(t))); }

        inline void operator=(const Nullable& other)
        {
            if (other.hasValue())
                *this = other.value();
            else
                reset();
        }

        inline void operator=(Nullable&& other)
            { _container = std::move(other._container); }



        Nullable()
            { }

        template<class... Args>
        Nullable(Args&&... args) :
            _container(new Container(std::forward<Args>(args)...))
            { }

        Nullable(Nullable& other)
            { if (static_cast<bool>(other)) *this = other(); }

        Nullable(const Nullable& other)
            { if (static_cast<bool>(other)) *this = other(); }

        Nullable(Nullable&& other) :
            _container(std::move(other._container))
            { }
    };

}