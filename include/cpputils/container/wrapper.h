#pragma once

namespace utl
{

    /* simple class that stores a value of type T */
    template<class T>
    struct wrapper
    {
        using value_type = T;

        value_type value;

        inline value_type& operator*()
            { return value; }

        inline const value_type& operator*() const
            { return value; }

        inline wrapper& operator=(value_type v)
        {
            value = v;
            return *this;
        }

        inline wrapper& operator=(const wrapper& other)
        {
            value = other.value;
            return *this;
        }

        inline wrapper& operator=(wrapper&& other)
        {
            value = std::move(other).value;
            return *this;
        }

        inline wrapper() :
            value(value_type())
            { }

        inline wrapper(const value_type& v) :
            value(v)
            { }

        inline wrapper(const wrapper& other) :
            value(other.value)
            { }

        inline wrapper(wrapper&& other) :
            value(std::move(other.value))
            { }
    };

    template<class T>
    struct wrapper<T&>
    {
        using value_type   = T&;
        using storage_type = T*;

        storage_type value;

        inline value_type operator*() const
        {
            assert(value != nullptr);
            return *value;
        }

        inline wrapper& operator=(value_type v)
        {
            value = &v;
            return *this;
        }

        inline wrapper& operator=(const wrapper& other)
        {
            value = other.value;
            return *this;
        }

        inline wrapper& operator=(wrapper&& other)
        {
            value = std::move(other.value);
            return *this;
        }

        inline wrapper() :
            value(nullptr)
            { }

        inline wrapper(value_type v) :
            value(&v)
            { }

        inline wrapper(const wrapper& other) :
            value(other.value)
            { }

        inline wrapper(wrapper&& other) :
            value(std::move(other.value))
            { }
    };

}