#pragma once

namespace utl
{

    namespace __impl
    {
        template<class It, class Tag, class T = void>
        using mp_enable_if_has_iterator_tag = typename std::enable_if<std::is_same<typename It::iterator_category, Tag>::value, T>::type;
    }

    template<class It, class Transform, class Enable = void>
    struct TransformIterator;

    template<class It, class Transform>
    struct TransformIterator<It, Transform, __impl::mp_enable_if_has_iterator_tag<It, std::forward_iterator_tag>>
    {
    public:
        using iterator_type         = TransformIterator;
        using iterator_category     = std::input_iterator_tag;
        using difference_type       = typename It::difference_type;
        using return_type           = decltype(std::declval<Transform>()(std::declval<typename It::reference>()));
        using value_type            = typename std::remove_reference<return_type>::type;
        using reference             = value_type&;
        using pointer               = value_type*;

    private:
        It          _it;
        Transform   _transform;

    public:
        TransformIterator()
        { }

        TransformIterator(It it, Transform transform) :
            _it         (it),
            _transform  (transform)
            { }

        inline iterator_type& operator=(const iterator_type& other)
        {
            _it = other._it;
            return *this;
        }

        inline iterator_type& operator++()
        {
            ++_it;
            return *this;
        }

        inline iterator_type operator++(int)
            { return iterator_type(_it++, _transform); }

        inline bool operator==(const iterator_type& other) const
            { return (_it == other._it) && (_transform == other._transform); }

        inline bool operator!= (const iterator_type& other) const
            { return (_it != other._it) || (_transform != other._transform); }

        inline return_type operator*()
            { return _transform(*_it); }

        inline pointer operator->()
            { return &_transform(*_it); }
    };

    template<class It, class Transform>
    struct TransformIterator<It, Transform, __impl::mp_enable_if_has_iterator_tag<It, std::bidirectional_iterator_tag>>
    {
    public:
        using iterator_type         = TransformIterator;
        using iterator_category     = std::input_iterator_tag;
        using difference_type       = typename It::difference_type;
        using return_type           = decltype(std::declval<Transform>()(std::declval<typename It::reference>()));
        using value_type            = typename std::remove_reference<return_type>::type;
        using reference             = value_type&;
        using pointer               = value_type*;

    private:
        It          _it;
        Transform   _transform;

    public:
        TransformIterator()
            { }

        TransformIterator(It it, Transform transform) :
            _it         (it),
            _transform  (transform)
            { }

        inline iterator_type& operator=(const iterator_type& other)
        {
            _it = other._it;
            return *this;
        }

        inline iterator_type& operator++()
        {
            ++_it;
            return *this;
        }

        inline iterator_type operator++(int)
            { return iterator_type(_it++, _transform); }

        inline iterator_type& operator--()
        {
            --_it;
            return *this;
        }

        inline iterator_type operator--(int)
            { return iterator_type(_it--, _transform); }

        inline bool operator==(const iterator_type& other) const
            { return (_it == other._it) && (_transform == other._transform); }

        inline bool operator!= (const iterator_type& other) const
            { return (_it != other._it) || (_transform != other._transform); }

        inline return_type operator*()
            { return _transform(*_it); }

        inline pointer operator->()
            { return &_transform(*_it); }
    };

    template<class It, class Transform>
    struct TransformIterator<It, Transform, __impl::mp_enable_if_has_iterator_tag<It, std::random_access_iterator_tag>>
    {
    public:
        using iterator_type         = TransformIterator;
        using iterator_category     = std::input_iterator_tag;
        using difference_type       = typename It::difference_type;
        using return_type           = decltype(std::declval<Transform>()(std::declval<typename It::reference>()));
        using value_type            = typename std::remove_reference<return_type>::type;
        using reference             = value_type&;
        using pointer               = value_type*;

    private:
        It          _it;
        Transform   _transform;

    public:
        TransformIterator()
            { }

        TransformIterator(It it, Transform transform) :
            _it         (it),
            _transform  (transform)
            { }

        inline iterator_type& operator=(const iterator_type& other)
        {
            _it = other._it;
            return *this;
        }

        inline iterator_type& operator++()
        {
            ++_it;
            return *this;
        }

        inline iterator_type operator++(int)
            { return iterator_type(_it++, _transform); }

        inline iterator_type& operator+=(difference_type n)
        {
            _it += n;
            return *this;
        }

        inline iterator_type operator+(difference_type n) const
            { return iterator_type(_it + n, _transform); }

        inline iterator_type& operator--()
        {
            --_it;
            return *this;
        }

        inline iterator_type operator--(int)
            { return iterator_type(_it--, _transform); }

        inline iterator_type& operator-=(difference_type n)
        {
            _it -= n;
            return *this;
        }

        inline iterator_type operator-(difference_type n) const
            { return iterator_type(_it - n); }

        inline difference_type operator-(const iterator_type& other) const
            { return _it - other._it; }

        inline bool operator==(const iterator_type& other) const
            { return (_it == other._it) && (_transform == other._transform); }

        inline bool operator!=(const iterator_type& other) const
            { return (_it != other._it) || (_transform != other._transform); }

        inline bool operator>(const iterator_type& other) const
            { return _it > other._it; }

        inline bool operator<(const iterator_type& other) const
            { return _it < other._it; }

        inline bool operator>=(const iterator_type& other) const
            { return _it >= other._it; }

        inline bool operator<=(const iterator_type& other) const
            { return _it <= other._it; }

        inline return_type operator*() const
            { return _transform(*_it); }

        inline pointer operator->() const
            { return &_transform(*_it); }

        inline return_type operator[](difference_type n) const
            { return *(*this + n); }
    };

    template<class It, class Transform>
    TransformIterator<It, Transform> makeTransformIterator(const It& it, const Transform& transform)
        { return TransformIterator<It, Transform>(it, transform); }

    template<class It, class Transform, __impl::mp_enable_if_has_iterator_tag<It, std::random_access_iterator_tag, int> = 0>
    inline TransformIterator<It, Transform>
    operator+(const typename TransformIterator<It, Transform>::difference_type& n, const TransformIterator<It, Transform>& it)
        { return it + n; }
}