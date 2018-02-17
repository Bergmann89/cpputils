#pragma once

#include <memory>
#include <type_traits>

#include <cpputils/misc/string.h>
#include <cpputils/misc/exception.h>

namespace utl
{

    template<class T>
    struct nullable
    {
    public:
        using value_type    = T;
        using clean_type    = typename std::remove_reference<T>::type;
        using pointer_type  = clean_type*;

    private:
        struct container
        {
            value_type value;

            template<class... Args>
            container(Args&&... args) :
                value(std::forward<Args>(args)...)
                { }
        };

    private:
        std::unique_ptr<container> _container;

        inline void check() const
        {
            if (!_container)
                throw invalid_operation_exception("nullable does not have a value");
        }

    public:
        inline value_type& value()
            { check(); return _container->value; }

        inline const value_type& value() const
            { check(); return _container->value; }

        inline bool has_value() const
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
            { return has_value(); }

        inline bool operator==(T v) const
            { return has_value() && value() == v; }

        inline bool operator==(const nullable& other) const
            { return has_value() && other.has_value() && value() == other.value(); }

        inline bool operator<(const nullable& other) const
        {
            return this->has_value()
                && other.has_value()
                && value() < other.value();
        }

        inline bool operator>(const nullable& other) const
        {
            return this->has_value()
                && other.has_value()
                && value() < other.value();
        }

        inline bool operator<=(const nullable& other) const
        {
            return this->has_value()
                && other.has_value()
                && value() < other.value();
        }

        inline bool operator>=(const nullable& other) const
        {
            return this->has_value()
                && other.has_value()
                && value() < other.value();
        }

        inline void operator=(value_type&& t)
            { _container.reset(new container(std::forward<value_type>(t))); }

        inline void operator=(const nullable& other)
        {
            if (other.has_value())
                *this = other.value();
            else
                reset();
        }

        inline void operator=(nullable&& other)
            { _container = std::move(other._container); }



        nullable()
            { }

        template<class... Args>
        nullable(Args&&... args) :
            _container(new container(std::forward<Args>(args)...))
            { }

        nullable(nullable& other)
            { if (static_cast<bool>(other)) *this = other(); }

        nullable(const nullable& other)
            { if (static_cast<bool>(other)) *this = other(); }

        nullable(nullable&& other) :
            _container(std::move(other._container))
            { }
    };

    namespace __impl
    {
        template<class T>
        struct op_to_string<nullable<T>, void>
        {
            inline void operator()(std::ostream& os, const nullable<T>& s) const
            {
                if (s.has_value())
                    op_to_string<T>()(os, s.value());
                else
                    os << "null";
            }
        };
    }

}