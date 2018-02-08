#pragma once

#include <set>
#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <functional>

#include "Nullable.h"
#include "Exceptions.h"
#include "MetaProgramming.h"

#define ISEF_LINQ_HAS_LAMBDA

namespace isef
{

    namespace linq
    {
    /* meta programming */
        template<class TContainer>
        struct mp_iterator_impl
        {
            static TContainer& get_container();
            using type = decltype(std::begin(get_container()));
        };

        template<class TContainer>
        using mp_iterator = typename mp_iterator_impl<TContainer>::type;



        template<class TBuilder, class TRange>
        struct mp_buildup_type_impl
        {
            static TBuilder get_builder();
            static TRange get_range();
            using type = decltype(get_builder().build(get_range()));
        };

        template<class TBuilder, class TRange>
        using mp_buildup_type = typename mp_buildup_type_impl<TBuilder, TRange>::type;



        template<class TRange>
        struct mp_range_value_type_impl
        {
            static TRange get_range();
            using type = decltype(get_range().front());
        };

        template<class TRange>
        using mp_range_value_type = typename mp_range_value_type_impl<TRange>::type;



        template<class TPredicate, class TParameter>
        struct mp_predicate_value_type_impl
        {
            static TPredicate get_predicate();
            static TParameter get_parameter();
            using type = decltype(get_predicate()(get_parameter()));
        };

        template<class op, class T>
        using mp_predicate_value_type = typename mp_predicate_value_type_impl<op, T>::type;



        template<class TArray>
        struct mp_array_properties;

        template<class T, size_t N>
        struct mp_array_properties<T[N]>
        {
            using size          = std::integral_constant<size_t, N>;
            using value_type    = T;
            using iterator_type = T*;
        };


        template<class T>
        using mp_cleanup_type = typename std::remove_const<
                                    typename std::remove_reference<T>::type
                                    >::type;



        template<class T>
        struct mp_is_pair : public isef::mp_false { };

        template<class TKey, class TValue>
        struct mp_is_pair<std::pair<TKey, TValue>> : public isef::mp_true { };



        template<class T>
        struct mp_is_tuple_of_two : public isef::mp_false { };

        template<class TKey, class TValue>
        struct mp_is_tuple_of_two<std::tuple<TKey, TValue>> : public isef::mp_true { };



    /* default operations */
        struct op_select_default
        {
            template<class T>
            inline T operator ()(T t) const
                { return t; }
        };

        struct op_select_pointer
        {
            template<class T>
            inline T* operator ()(T& t) const
                { return &t; }
        };

        struct op_compare_default
        {
            template<class T>
            inline bool operator()(const T& l, const T& r) const
                { return (l == r); }
        };

        struct op_less_default
        {
            template<class T>
            inline bool operator()(const T& l, const T& r) const
                { return (l < r); }
        };

        struct op_auto_select_key
        {
            template<class TKey, class TValue>
            inline TKey operator()(const std::pair<TKey, TValue>& p) const
                { return p.first; }

            template<class TKey, class TValue>
            inline TKey operator()(const std::tuple<TKey, TValue>& t) const
                { return std::get<0>(t); }

            template<class T>
            inline typename std::enable_if<mp_and<
                mp_bool<!mp_is_pair<T>::value>,
                mp_bool<!mp_is_tuple_of_two<T>::value>
            >::value, T>::type
            operator()(const T& t) const
                { return t; }
        };

        struct op_auto_select_value
        {
            template<class TKey, class TValue>
            inline TValue operator()(const std::pair<TKey, TValue>& p) const
                { return p.second; }

            template<class TKey, class TValue>
            inline TValue operator()(const std::tuple<TKey, TValue>& t) const
                { return std::get<1>(t); }

            template<class T>
            inline typename std::enable_if<mp_and<
                mp_bool<!mp_is_pair<T>::value>,
                mp_bool<!mp_is_tuple_of_two<T>::value>
            >::value, T>::type
            operator()(const T& t) const
                { return t; }
        };

    /* types */
        struct range_tag
            { };

        struct builder_tag
            { };

        template<class TKey, class TValue>
        struct lookup
        {
        public:
            using this_type   = lookup<TKey, TValue>;

            using key_type    = TKey;
            using value_type  = TValue;
            using return_type = const value_type&;

            using keys_type   = std::vector<std::pair<key_type, size_t>>;
            using values_type = std::vector<value_type>;

            struct lookup_range : public range_tag
            {
                using this_type = lookup_range;

                enum class State
                {
                    Initialize,
                    Iterating,
                    Finished
                };

                const values_type*  values;
                size_t              it;
                size_t              end;
                State               state;

                inline return_type front() const
                {
                    assert(state == State::Iterating);
                    assert(it < end);
                    return (*values)[it];
                }

                inline bool next()
                {
                    switch (state)
                    {
                        case State::Iterating:
                            ++it;
                        case State::Initialize:
                        {
                            auto has_elements = (it < end);
                            state = has_elements ? State::Iterating : State::Finished;
                            return has_elements;
                        }
                        break;

                        case State::Finished:
                        default:
                            return false;
                    }
                }

                template<class TBuilder>
                inline mp_buildup_type<TBuilder, this_type>
                operator >> (TBuilder builder) const
                    { return builder.build(*this); }

                lookup_range() :
                    values  (nullptr),
                    it      (0),
                    end     (0),
                    state   (State::Finished)
                    { }

                lookup_range(
                    const values_type*  v,
                    size_t              i,
                    size_t              e) :
                        values  (v),
                        it      (i),
                        end     (e),
                        state   (State::Initialize)
                    { assert(values); }

                lookup_range(const lookup_range& other) :
                    values  (other.values),
                    it      (other.it),
                    end     (other.end),
                    state   (other.state)
                    { }

                lookup_range(lookup_range&& other) :
                    values  (std::move(other.values)),
                    it      (std::move(other.it)),
                    end     (std::move(other.end)),
                    state   (std::move(other.state))
                    { }
            };

            struct lookup_key_value_range : public range_tag
            {
                using this_type     = lookup_key_value_range;
                using iterator_type = typename keys_type::const_iterator;
                using value_type    = std::pair<TKey, lookup_range>;
                using storage_type  = nullable<value_type>;
                using return_type   = value_type&;

                const lookup&       container;
                const keys_type&    keys;
                bool                initialized;
                storage_type        storage;
                iterator_type       current;
                iterator_type       end;

                inline return_type front()
                {
                    assert(storage);
                    assert(current != end);
                    return *storage;
                }

                inline bool next()
                {
                    if (!initialized)
                    {
                        initialized = true;
                        current     = keys.begin();
                        end         = keys.end();
                        if (current == end)
                            return false;
                    }
                    else
                    {
                        if (current == end)
                            return false;
                        ++current;
                        if (current == end)
                            return false;
                    }
                    storage = value_type(current->first, container[current->first]);
                    return true;
                }

                lookup_key_value_range(const lookup& l, const keys_type& k) :
                    container   (l),
                    keys        (k),
                    initialized (false)
                    { }

                lookup_key_value_range(const lookup_key_value_range& other) :
                    container   (other.container),
                    keys        (other.keys),
                    current     (other.current),
                    end         (other.end),
                    initialized (other.initialized)
                    { }

                lookup_key_value_range(lookup_key_value_range&& other) :
                    container   (std::move(other.container)),
                    keys        (std::move(other.keys)),
                    current     (std::move(other.current)),
                    end         (std::move(other.end)),
                    initialized (std::move(other.initialized))
                    { }
            };

            struct op_compare_keys
            {
                bool operator()(
                    const typename keys_type::value_type& l,
                    const typename keys_type::value_type& r)
                    { return l.first < r.first; }
            };

        private:
            keys_type   _keys;
            values_type _values;

        public:
            inline void swap(lookup& other)
            {
                _keys.swap  (other._keys);
                _values.swap(other._values);
            }

            lookup& operator= (const lookup& other)
            {
                if (std::addressof(other) == this)
                    return *this;
                lookup tmp(other);
                swap(tmp);
                return *this;
            }

            lookup& operator= (lookup&& other)
            {
                if (std::addressof(other) == this)
                    return *this;
                swap(other);
                return *this;
            }

            lookup_range operator[] (const key_type& key) const
            {
                if (_values.empty())
                    return lookup_range(std::addressof(_values), 0, 0);

                auto it = std::lower_bound(
                                _keys.begin(),
                                _keys.end(),
                                typename keys_type::value_type (key, 0),
                                op_compare_keys());
                if (    it == _keys.end()
                    ||  it->first != key)
                    return lookup_range(std::addressof(_values), 0, 0);

                auto next = it + 1;
                if (next == _keys.end())
                    return lookup_range(std::addressof(_values), it->second, _values.size());

                return lookup_range(std::addressof(_values), it->second, next->second);
            }

            template<class TBuilder>
            inline mp_buildup_type<TBuilder, lookup_key_value_range>
            operator >> (TBuilder builder) const
                { return builder.build(lookup_key_value_range(*this, _keys)); }

            template<class TRange, class TKeyPredicate, class TValuePredicate>
            lookup(
                size_t          capacity,
                TRange&         range,
                TKeyPredicate   keyPredicate,
                TValuePredicate valuePredicate)
            {
                keys_type   k;
                values_type v;
                k.reserve(capacity);
                v.reserve(capacity);

                size_t index = 0;
                while (range.next())
                {
                    auto& tmp = range.front();
                    k.emplace_back(std::move(keyPredicate  (tmp)), index);
                    v.emplace_back(std::move(valuePredicate(tmp)));
                    ++index;
                }

                if (v.empty())
                    return;

                std::sort(k.begin(), k.end(), op_compare_keys());

                _keys.reserve  (k.size());
                _values.reserve(k.size());

                auto it  = k.begin();
                auto end = k.end();
                index    = 0;

                if (it != end)
                {
                    _keys.push_back  (std::make_pair(it->first, index));
                    _values.push_back(std::move(v.at(it->second)));
                }

                auto prev = it;
                ++it;
                ++index;

                while (it != end)
                {
                    _values.push_back(v.at(it->second));
                    if (prev->first < it->first)
                        _keys.push_back(std::make_pair(it->first, index));
                    prev = it;
                    ++it;
                    ++index;
                }
            }

            lookup()
                { }

            lookup(const lookup& other) :
                _keys   (other._keys),
                _values (other._values)
                { }

            lookup(lookup&& other) :
                _keys   (std::move(other._keys)),
                _values (std::move(other._values))
                { }
        };

        template<typename TRange>
        struct container_iterator
        {
            static TRange get_range();

            using iterator_category = std::forward_iterator_tag;
            using value_type        = mp_cleanup_type<decltype(get_range().front())>;
            using return_type       = decltype(get_range().front());

            using difference_type   = std::ptrdiff_t;
            using pointer           = value_type*;
            using reference         = value_type&;

            using this_type         = container_iterator<TRange>;
            using range_type        = TRange;

            nullable<range_type>    range;
            bool                    hasValue;

            inline return_type operator* ()
            {
                assert(hasValue);
                assert(range);
                return range->front();
            }

            inline const value_type* operator-> ()
                { return &range->front(); }

            inline this_type& operator++()
            {
                if (hasValue && range)
                    hasValue = range->next();
                return *this;
            }

            inline bool operator== (const this_type& other) const
            {
                if (!hasValue && !other.hasValue)
                    return true;
                else if (hasValue && other.hasValue && range.ptr() == other.range.ptr())
                    return true;
                else
                    return false;
            }

            inline bool operator!= (const this_type& other) const
                { return !(*this == other); }

            container_iterator() :
                hasValue(false)
                { }

            container_iterator(range_type r) :
                range(std::move(r))
                { hasValue = range && range->next(); }

            container_iterator(const container_iterator& other) :
                hasValue(other.hasValue),
                range   (other.range)
                { }

            container_iterator(container_iterator&& other) :
                hasValue(std::move(other.hasValue)),
                range   (std::move(other.range))
                { }
        };

        template<class TRange>
        struct container
        {
            TRange range;

            container_iterator<TRange> begin()
                { return container_iterator<TRange>(range); }

            container_iterator<TRange> end()
                { return container_iterator<TRange>(); }

            explicit container(TRange r) :
                range(std::move(r))
                { }

            container(const container& other) :
                range(other.range)
                { }

            container(container&& other) :
                range(std::move(other.range))
                { }
        };

    /* ranges */
        template<class TIterator>
        struct from_range : public range_tag
        {
            static TIterator get_iterator();

            using this_type     = from_range<TIterator>;
            using iterator_type = TIterator;
            using value_type    = decltype(*get_iterator());
            using return_type   = value_type&;

            bool          initialized;
            iterator_type current;
            iterator_type end;

            inline return_type front() const
            {
                assert(current != end);
                return *current;
            }

            inline bool next()
            {
                if (!initialized)
                    initialized = true;
                else if (current != end)
                    ++current;
                return (current != end);
            }

            template<class TBuilder>
            mp_buildup_type<TBuilder, this_type>
            operator >> (TBuilder builder) const
                { return builder.build(*this); }

            from_range(iterator_type b, iterator_type e) :
                initialized (false),
                current     (b),
                end         (e)
                { }

            from_range(const from_range& other) :
                initialized (other.initialized),
                current     (other.current),
                end         (other.end)
                { }

            from_range(from_range&& other) :
                initialized (std::move(other.initialized)),
                current     (std::move(other.current)),
                end         (std::move(other.end))
                { }
        };

        template<class TContainer>
        struct container_range : public range_tag
        {
            static TContainer& get_container();

            using this_type      = container_range<TContainer>;
            using container_type = TContainer;
            using iterator_type  = decltype(get_container().begin());
            using value_type     = decltype(*get_container().begin());
            using return_type    = value_type&;

            TContainer&   container;
            bool          initialized;
            iterator_type current;
            iterator_type end;

            inline return_type front() const
            {
                assert(initialized);
                assert(current != end);
                return *current;
            }

            inline bool next()
            {
                if (!initialized)
                {
                    initialized = true;
                    current     = container.begin();
                    end         = container.end();
                }
                else if (current != end)
                    ++current;
                return (current != end);
            }

            template<class TBuilder>
            mp_buildup_type<TBuilder, this_type>
            operator >> (TBuilder builder) const
                { return builder.build(*this); }

            container_range(TContainer& c) :
                initialized(false),
                container  (c)
                { }

            container_range(const container_range& other) :
                initialized (other.initialized),
                container   (other.container),
                current     (other.current),
                end         (other.end)
                { }

            container_range(container_range&& other) :
                initialized (std::move(other.initialized)),
                container   (          other.container),
                current     (std::move(other.current)),
                end         (std::move(other.end))
                { }
        };

        template<class TPredicate>
        struct generate_range :  public range_tag
        {
            static TPredicate get_gnerator();

            using this_type               = generate_range<TPredicate>;
            using raw_nullable_value_type = decltype(get_gnerator()());
            using nullable_value_type     = mp_cleanup_type<raw_nullable_value_type>;
            using raw_value_type          = decltype(*(get_gnerator()()));
            using value_type              = mp_cleanup_type<raw_value_type>;
            using predicate_type          = TPredicate;
            using return_type             = const value_type&;

            predicate_type          predicate;
            nullable_value_type     value;

            inline return_type front() const
            {
                assert(value);
                return *value;
            }

            inline bool next()
            {
                value = predicate();
                return value.hasValue();
            }

            template<class TBuilder>
            mp_buildup_type<TBuilder, this_type>
            operator >> (TBuilder builder) const
                { return builder.build(*this); }

            generate_range(predicate_type p) :
                predicate(p)
                { }

            generate_range(const generate_range& other) :
                value       (other.value),
                predicate   (other.predicate)
                { }

            generate_range(generate_range&& other) :
                value       (std::move(other.value)),
                predicate   (std::move(other.predicate))
                { }
        };

        template<class TRange, class TPredicate>
        struct where_range : public range_tag
        {
            static TRange get_range();

            using this_type      = where_range<TRange, TPredicate>;
            using range_type     = TRange;
            using predicate_type = TPredicate;
            using value_type     = decltype(get_range().front());
            using return_type    = value_type&;

            range_type      range;
            predicate_type  predicate;

            inline return_type front()
                { return range.front(); }

            inline bool next()
            {
                while (range.next())
                {
                    if (predicate(range.front()))
                        return true;
                }
                return false;
            }

            template<class TBuilder>
            mp_buildup_type<TBuilder, this_type>
            operator >> (TBuilder builder) const
                { return builder.build(*this); }

            where_range(range_type r, predicate_type p) :
                predicate   (p),
                range       (r)
                { }

            where_range(const where_range& other) :
                predicate   (other.predicate),
                range       (other.range)
                { }

            where_range(where_range&& other) :
                predicate   (std::move(other.predicate)),
                range       (std::move(other.range))
                { }
        };

        template<class TRange, class TPredicate>
        struct select_range : public range_tag
        {
            static TRange get_range();

            using this_type        = select_range<TRange, TPredicate>;
            using range_type       = TRange;
            using predicate_type   = TPredicate;
            using range_value_type = decltype(get_range().front());

            static predicate_type   get_predicate();
            static range_value_type get_range_value();
            using value_type       = decltype(get_predicate()(get_range_value()));
            using return_type      = value_type&;

            predicate_type          predicate;
            range_type              range;
            nullable<value_type>    cache;

            inline return_type front()
            {
                assert(cache);
                return *cache;
            }

            inline bool next()
            {
                if (range.next())
                {
                    cache = predicate(range.front());
                    return true;
                }
                cache.clear();
                return false;
            }

            template<class TBuilder>
            mp_buildup_type<TBuilder, this_type>
            operator >> (TBuilder builder) const
                { return builder.build(*this); }

            select_range(range_type r, predicate_type p) :
                predicate   (p),
                range       (r)
                { }

            select_range(const select_range& other) :
                predicate   (other.predicate),
                range       (other.range)
                { }

            select_range(select_range&& other) :
                predicate   (std::move(other.predicate)),
                range       (std::move(other.range))
                { }
        };

        template<class TRange, class TPredicate>
        struct select_many_range : public range_tag
        {
            template<class T>
            struct mp_make_inner_range_impl
            {
                static T get_T();
                using iterator_type = decltype(std::begin(get_T()));
                using type          = from_range<iterator_type>;
            };

            template<class T>
            using mp_make_inner_range = typename mp_make_inner_range_impl<T>::type;


            static TRange get_range();
            using this_type         =   select_many_range<TRange, TPredicate>;
            using range_type        =   TRange;
            using predicate_type    =   TPredicate;
            using range_value_type  =   decltype(get_range().front());

            static predicate_type   get_predicate();
            static range_value_type get_range_value();
            using op_return_type    =   decltype(get_predicate()(get_range_value()));

            using inner_range_type  =   isef::mp_eval_if<
                                            std::is_base_of<range_tag, op_return_type>,
                                            op_return_type,
                                            mp_make_inner_range, op_return_type
                                        >;

            static inner_range_type get_inner_range();
            using return_type       =   decltype(get_inner_range().front());

            predicate_type              predicate;
            range_type                  range;
            nullable<op_return_type>    cache;
            nullable<inner_range_type>  inner_range;

            template<class T>
            inline typename std::enable_if<std::is_base_of<range_tag, T>::value>::type
            build_inner_range(T value)
                { inner_range = value; }

            template<class T>
            inline typename std::enable_if<!std::is_base_of<range_tag, T>::value>::type
            build_inner_range(const T& value)
                { inner_range = inner_range_type(std::begin(value), std::end(value)); }

            inline return_type front()
            {
                assert(cache);
                assert(inner_range);
                return inner_range->front();
            }

            inline bool next()
            {
                if (cache && inner_range && inner_range->next())
                    return true;

                while (range.next())
                {
                    inner_range.clear();
                    cache = predicate(range.front());
                    if (cache)
                        build_inner_range<op_return_type>(*cache);
                    if (cache && inner_range && inner_range->next())
                        return true;
                }
                cache.clear();
                inner_range.clear();
                return false;
            }

            template<class TBuilder>
            mp_buildup_type<TBuilder, this_type>
            operator >> (TBuilder builder) const
                { return builder.build(*this); }

            select_many_range(range_type r, predicate_type p) :
                predicate   (p),
                range       (r)
                { }

            select_many_range(const select_many_range& other) :
                predicate   (other.predicate),
                range       (other.range)
                { }

            select_many_range(select_many_range&& other) :
                predicate   (std::move(other.predicate)),
                range       (std::move(other.range))
                { }
        };

        template<class TRange, class TSelectPredicate, class TLessPredicate>
        struct order_by_range : public range_tag
        {
            using this_type             = order_by_range<TRange, TSelectPredicate, TLessPredicate>;
            using range_type            = TRange;
            using select_predicate_type = TSelectPredicate;
            using less_predicate_type   = TLessPredicate;
            using return_type           = mp_range_value_type<range_type>;
            using value_type            = mp_if<
                                            std::is_reference<return_type>,
                                            typename std::remove_reference<return_type>::type*,
                                            return_type>;

            range_type              range;
            select_predicate_type   select_predicate;
            less_predicate_type     less_predicate;
            ssize_t                 current;
            std::vector<value_type> values;

            template<class X>
            inline mp_enable_if<std::is_reference<X>::value, X>
            front_impl()
            {
                assert(current >= 0);
                assert(current < static_cast<ssize_t>(values.size()));
                return *values.at(current);
            }

            template<class X>
            inline mp_enable_if<!std::is_reference<X>::value, X>
            front_impl()
            {
                assert(current >= 0);
                assert(current < static_cast<ssize_t>(values.size()));
                return values.at(current);
            }

            template<class X>
            inline mp_enable_if<std::is_reference<X>::value>
            storeValue(X x)
                { values.push_back(&x); }

            template<class X>
            inline mp_enable_if<!std::is_reference<X>::value>
            storeValue(X x)
                { values.emplace(x); }

            template<class X>
            inline mp_enable_if<std::is_reference<X>::value>
            sortValues()
            {
                std::sort(
                    values.begin(),
                    values.end(),
                    [this](const value_type& l, const value_type& r) {
                        return this->less_predicate(this->select_predicate(*l), this->select_predicate(*r));
                    });
            }

            template<class X>
            inline mp_enable_if<!std::is_reference<X>::value>
            sortValues()
            {
                std::sort(
                    values.begin(),
                    values.end(),
                    [this](const value_type& l, const value_type& r) {
                        return this->less_predicate(this->select_predicate(l), this->select_predicate(r));
                    });
            }

            inline return_type front()
                { return front_impl<return_type>(); }

            inline bool next()
            {
                if (current < 0)
                {
                    values.clear();
                    while (range.next())
                        storeValue<return_type>(range.front());

                    if (values.empty())
                        return false;

                    sortValues<return_type>();

                    current = 0;
                    return true;
                }

                if (current < static_cast<ssize_t>(values.size()))
                    ++current;
                return (current < static_cast<ssize_t>(values.size()));
            }

            template<class TBuilder>
            mp_buildup_type<TBuilder, this_type>
            operator >> (TBuilder builder) const
                { return builder.build(*this); }

            order_by_range(range_type r, select_predicate_type sp, less_predicate_type lp) :
                range           (r),
                select_predicate(sp),
                less_predicate  (lp),
                current         (-1)
                { }

            order_by_range(const this_type& other) :
                range           (other.range),
                select_predicate(other.select_predicate),
                less_predicate  (other.less_predicate),
                current         (other.current)
                { }

            order_by_range(this_type&& other) :
                range           (std::move(other.range)),
                select_predicate(std::move(other.select_predicate)),
                less_predicate  (std::move(other.less_predicate)),
                current         (std::move(other.current))
                { }
        };

        template<class TRange>
        struct distinct_range : public range_tag
        {
            using this_type     = distinct_range<TRange>;
            using range_type    = TRange;
            using return_type   = mp_range_value_type<range_type>;
            using value_type    = mp_cleanup_type<return_type>;
            using set_type      = std::set<value_type>;
            using cache_type    = nullable<return_type>;

            range_type          range;
            set_type            set;
            cache_type          cache;

            inline return_type front()
            {
                assert(static_cast<bool>(cache));
                return *cache;
            }

            inline bool next()
            {
                while (range.next())
                {
                    auto ret = set.insert(range.front());
                    if (ret.second)
                    {
                        cache = range.front();
                        return true;
                    }
                }
                return false;
            }

            template<class TBuilder>
            mp_buildup_type<TBuilder, this_type>
            operator >> (TBuilder builder) const
                { return builder.build(*this); }

            distinct_range(range_type r) :
                range(r)
                { }

            distinct_range(const this_type& other) :
                range(other.range),
                set  (other.set),
                cache(other.cache)
                { }

            distinct_range(this_type&& other) :
                range(std::move(other.range)),
                set  (std::move(other.set)),
                cache(std::move(other.cache))
                { }
        };

        template <class TRange, class T>
        struct default_if_empty_range : public range_tag
        {
            using this_type   = default_if_empty_range<TRange, T>;
            using range_type  = TRange;
            using value_type  = T;
            using return_type = const T&;

            enum class State
            {
                Init,
                Iterate,
                Empty,
                Finish
            };

            range_type range;
            value_type value;
            State      state;

            inline return_type front()
            {
                assert(state != State::Init && state != State::Finish);
                if (state == State::Empty)
                    return value;
                else
                    return range.front();
            }

            inline bool next()
            {
                bool ret = false;
                switch(state)
                {
                    case State::Init:
                        ret = range.next();
                        if (ret)
                        {
                            state = State::Iterate;
                        }
                        else
                        {
                            state = State::Empty;
                            ret = true;
                        }
                        break;

                    case State::Iterate:
                        ret = range.next();
                        if (!ret)
                            state = State::Finish;
                        break;

                    case State::Empty:
                        ret = false;
                        state = State::Finish;
                        break;

                    default:
                        break;
                }
                return ret;
            }

            template<class TBuilder>
            mp_buildup_type<TBuilder, this_type>
            operator >> (TBuilder builder) const
                { return builder.build(*this); }

            default_if_empty_range(range_type r, T t) :
                range(r),
                value(t),
                state(State::Init)
                { }

            default_if_empty_range(const default_if_empty_range& other) :
                range(other.range),
                value(other.value),
                state(other.state)
                { }

            default_if_empty_range(default_if_empty_range&& other) :
                range(std::move(other.range)),
                value(std::move(other.value)),
                state(std::move(other.state))
                { }
        };

    /* range builder */
        template<class TPredicate>
        struct where_builder : public builder_tag
        {
            using this_type      = where_builder<TPredicate>;
            using predicate_type = TPredicate;

            predicate_type predicate;

            template<class TRange>
            where_range<TRange, predicate_type>
            build(TRange range) const
                { return where_range<TRange, predicate_type>(range, predicate); }

            where_builder(predicate_type p) :
                predicate(p)
                { }

            where_builder(const this_type& other) :
                predicate(other.predicate)
                { }

            where_builder(const this_type&& other) :
                predicate(std::move(other.predicate))
                { }
        };

        template<class TPredicate>
        struct select_builder : public builder_tag
        {
            using this_type      = select_builder<TPredicate>;
            using predicate_type = TPredicate;

            predicate_type predicate;

            template<class TRange>
            select_range<TRange, predicate_type>
            build(TRange range) const
                { return select_range<TRange, predicate_type>(range, predicate); }

            select_builder(predicate_type p) :
                predicate(p)
                { }

            select_builder(const this_type& other) :
                predicate(other.predicate)
                { }

            select_builder(this_type&& other) :
                predicate(std::move(other.predicate))
                { }
        };

        template<class TPredicate>
        struct select_many_builder : public builder_tag
        {
            using this_type      = select_many_builder<TPredicate>;
            using predicate_type = TPredicate;

            predicate_type predicate;

            template<class TRange>
            select_many_range<TRange, predicate_type>
            build(TRange range) const
                { return select_many_range<TRange, predicate_type>(range, predicate); }

            select_many_builder(predicate_type p) :
                predicate(p)
                { }

            select_many_builder(const this_type& other) :
                predicate(other.predicate)
                { }

            select_many_builder(this_type&& other) :
                predicate(std::move(other.predicate))
                { }
        };

        template<class TSelectPredicate, class TLessPredicate>
        struct order_by_builder : public builder_tag
        {
            using this_type             = order_by_builder<TSelectPredicate, TLessPredicate>;
            using select_predicate_type = TSelectPredicate;
            using less_predicate_type   = TLessPredicate;

            select_predicate_type   select_predicate;
            less_predicate_type     less_predicate;

            template<class TRange>
            order_by_range<TRange, select_predicate_type, less_predicate_type>
            build(TRange range) const
                { return order_by_range<TRange, select_predicate_type, less_predicate_type>(std::move(range), select_predicate, less_predicate); }

            order_by_builder(select_predicate_type sp, less_predicate_type lp) :
                select_predicate(sp),
                less_predicate  (lp)
                { }

            order_by_builder(const this_type& other) :
                select_predicate(other.select_predicate),
                less_predicate  (other.less_predicate)
                { }

            order_by_builder(this_type&& other) :
                select_predicate(std::move(other.select_predicate)),
                less_predicate  (std::move(other.less_predicate))
                { }
        };

        struct distinct_builder : public builder_tag
        {
            using this_type = distinct_builder;

            template<class TRange>
            inline distinct_range<TRange>
            build(TRange range) const
                { return distinct_range<TRange>(range); }

            distinct_builder()
                { }

            distinct_builder(const this_type& other)
                { }

            distinct_builder(this_type&& other)
                { }
        };

        template <class T>
        struct default_if_empty_builder : public builder_tag
        {
            using this_type = default_if_empty_builder<T>;

            T value;

            template<class TRange>
            inline default_if_empty_range<TRange, T>
            build(TRange range) const
                { return default_if_empty_range<TRange, T>(range, value); }

            default_if_empty_builder(T t) :
                value(t)
                { }

            default_if_empty_builder(const this_type& other) :
                value(other.value)
                { }

            default_if_empty_builder(this_type&& other) :
                value(std::move(other.value))
                { }
        };

    /* result builder */
        struct sum_builder : public builder_tag
        {
            template<class TRange>
            mp_cleanup_type<mp_range_value_type<TRange>>
            build(TRange range) const
            {
                auto sum = mp_cleanup_type<mp_range_value_type<TRange>>();
                while (range.next())
                    sum += range.front();
                return sum;
            }
        };

        template<class T, class TPredicate>
        struct contains_builder : public builder_tag
        {
            using this_type         = contains_builder<T, TPredicate>;
            using comparator_type   = T;
            using predicate_type    = TPredicate;

            comparator_type comparator;
            predicate_type  predicate;

            template<class TRange>
            bool build(TRange range) const
            {
                while (range.next())
                {
                    if (predicate(comparator, range.front()))
                        return true;
                }
                return false;
            }

            contains_builder(comparator_type t, predicate_type p) :
                comparator  (t),
                predicate   (p)
                { }

            contains_builder(const this_type& other) :
                comparator  (other.comparator),
                predicate   (other.predicate)
                { }

            contains_builder(this_type&& other) :
                comparator  (std::move(other.comparator)),
                predicate   (std::move(other.predicate))
                { }
        };

        struct single_or_default_builder : public builder_tag
        {
            template<class TRange>
            inline mp_cleanup_type<mp_range_value_type<TRange>>
            build(TRange range) const
            {
                using return_type = mp_cleanup_type<mp_range_value_type<TRange>>;

                if (!range.next())
                    return return_type();
                auto ret = range.front();
                if (range.next())
                    return return_type();
                return ret;
            }
        };

        struct first_builder : public builder_tag
        {
            template<class TRange>
            inline mp_range_value_type<TRange>
            build(TRange range) const
            {
                if (!range.next())
                    throw isef::Exception("range is empty");
                return range.front();
            }
        };

        struct first_or_default_builder : public builder_tag
        {
            template<class TRange>
            inline mp_cleanup_type<mp_range_value_type<TRange>>
            build(TRange range) const
            {
                using return_type = mp_cleanup_type<mp_range_value_type<TRange>>;

                if (!range.next())
                    return return_type();
                return range.front();
            }
        };

        struct last_builder : public builder_tag
        {
            template<class TRange>
            inline mp_range_value_type<TRange>
            build(TRange range) const
            {
                using return_type = mp_range_value_type<TRange>;

                nullable<return_type> cache;
                while (range.next())
                    cache = range.front();
                if (!static_cast<bool>(cache))
                    throw isef::Exception("range is empty");
                return *cache;
            }
        };

        struct last_or_default_builder : public builder_tag
        {
            template<class TRange>
            inline mp_cleanup_type<mp_range_value_type<TRange>>
            build(TRange range) const
            {
                using return_type = mp_range_value_type<TRange>;
                using value_type  = mp_cleanup_type<return_type>;

                nullable<return_type> cache;
                while (range.next())
                    cache = range.front();

                return static_cast<bool>(cache) ? *cache : value_type();
            }
        };

        struct any_builder : public builder_tag
        {
            template<class TRange>
            inline bool build(TRange range) const
                { return range.next(); }
        };

        struct min_builder : public builder_tag
        {
            template<class TRange>
            inline mp_cleanup_type<mp_range_value_type<TRange>>
            build(TRange range) const
            {
                using return_type = mp_cleanup_type<mp_range_value_type<TRange>>;
                return_type ret = std::numeric_limits<return_type>::max();
                while (range.next())
                {
                    if (ret > range.front())
                        ret = range.front();
                }
                return ret;
            }
        };

        struct max_builder : public builder_tag
        {
            template<class TRange>
            inline mp_cleanup_type<mp_range_value_type<TRange>>
            build(TRange range) const
            {
                using return_type = mp_cleanup_type<mp_range_value_type<TRange>>;
                return_type ret = std::numeric_limits<return_type>::min();
                while (range.next())
                {
                    if (ret < range.front())
                        ret = range.front();
                }
                return ret;
            }
        };

        struct to_vector_builder : public builder_tag
        {
            size_t capacity;

            template<class TRange>
            std::vector<mp_cleanup_type<mp_range_value_type<TRange>>>
            build(TRange range) const
            {
                std::vector<mp_cleanup_type<mp_range_value_type<TRange>>> ret;
                ret.reserve(capacity);
                while (range.next())
                    ret.emplace_back(range.front());
                return ret;
            }

            to_vector_builder(size_t cap = 16) :
                capacity(cap)
                { }
        };

        struct to_list_builder : public builder_tag
        {
            template<class TRange>
            std::list<mp_cleanup_type<mp_range_value_type<TRange>>>
            build(TRange range) const
            {
                std::list<mp_cleanup_type<mp_range_value_type<TRange>>> ret;
                while (range.next())
                    ret.push_back(range.front());
                return ret;
            }
        };

        template<class TKeyPredicate, class TValuePredicate>
        struct to_map_builder : public builder_tag
        {
            using this_type             = to_map_builder<TKeyPredicate, TValuePredicate>;
            using key_predicate_type    = TKeyPredicate;
            using value_predicate_type  = TValuePredicate;

            key_predicate_type      keyPredicate;
            value_predicate_type    valuePredicate;

            template<class TRange>
            std::map<
                mp_predicate_value_type<key_predicate_type,   mp_range_value_type<TRange>>,
                mp_predicate_value_type<value_predicate_type, mp_range_value_type<TRange>>>
            build(TRange range) const
            {
                using map_type = std::map<
                    mp_predicate_value_type<key_predicate_type,   mp_range_value_type<TRange>>,
                    mp_predicate_value_type<value_predicate_type, mp_range_value_type<TRange>>>;
                map_type map;
                while (range.next())
                {
                    auto& tmp = range.front();
                    auto ret = map.emplace(
                        std::move(keyPredicate  (tmp)),
                        std::move(valuePredicate(tmp)));
                    if (!ret.second)
                        throw isef::Exception("duplicate key");
                }
                return map;
            }

            to_map_builder(key_predicate_type k, value_predicate_type v) :
                keyPredicate    (k),
                valuePredicate  (v)
                { }

            to_map_builder(const this_type& other) :
                keyPredicate    (other.keyPredicate),
                valuePredicate  (other.valuePredicate)
                { }

            to_map_builder(this_type&& other) :
                keyPredicate    (std::move(other.keyPredicate)),
                valuePredicate  (std::move(other.valuePredicate))
                { }
        };

        template<class TKeyPredicate, class TValuePredicate>
        struct to_lookup_builder  : public builder_tag
        {
            using this_type             = to_lookup_builder<TKeyPredicate, TValuePredicate>;
            using key_predicate_type    = TKeyPredicate;
            using value_predicate_type  = TValuePredicate;

            key_predicate_type      keyPredicate;
            value_predicate_type    valuePredicate;

            template<class TRange>
            lookup<
                mp_predicate_value_type<key_predicate_type,   mp_range_value_type<TRange>>,
                mp_predicate_value_type<value_predicate_type, mp_range_value_type<TRange>>>
            build(TRange range) const
            {
                using lookup_type = lookup<
                    mp_predicate_value_type<key_predicate_type,   mp_range_value_type<TRange>>,
                    mp_predicate_value_type<value_predicate_type, mp_range_value_type<TRange>>>;
                return lookup_type(16, range, keyPredicate, valuePredicate);
            }

            to_lookup_builder(key_predicate_type k, value_predicate_type v) :
                keyPredicate    (k),
                valuePredicate  (v)
                { }

            to_lookup_builder(const this_type& other) :
                keyPredicate    (other.keyPredicate),
                valuePredicate  (other.valuePredicate)
                { }

            to_lookup_builder(this_type&& other) :
                keyPredicate    (std::move(other.keyPredicate)),
                valuePredicate  (std::move(other.valuePredicate))
                { }
        };

        template<class TPredicate>
        struct for_each_builder : public builder_tag
        {
            using this_type      = for_each_builder<TPredicate>;
            using predicate_type = TPredicate;

            predicate_type predicate;

            template<class TRange>
            void build(TRange range) const
            {
                while(range.next())
                    predicate(range.front());
            }

            for_each_builder(predicate_type p) :
                predicate(p)
                { }

            for_each_builder(const this_type& other) :
                predicate(other.predicate)
                { }

            for_each_builder(this_type&& other) :
                predicate(std::move(other.predicate))
                { }
        };

        struct container_builder
        {
            template<class TRange>
            inline container<TRange>
            build(TRange range) const
                { return container<TRange>(range); }
        };
    };

/* constructors */
    template<class TIterator>
    inline linq::from_range<TIterator>
    from_iterators(TIterator beg, TIterator end)
        { return linq::from_range<TIterator>(beg, end); }

    template<class TContainer>
    inline linq::container_range<TContainer>
    from_container(TContainer& c)
        { return linq::container_range<TContainer>(c); }

    template<class TArray>
    inline linq::from_range<typename linq::mp_array_properties<TArray>::iterator_type>
    from_array(TArray& array)
    {
        using properties    = typename linq::mp_array_properties<TArray>;
        using iterator_type = typename properties::iterator_type;
        using range_type    = typename linq::from_range<iterator_type>;

        iterator_type beg = array;
        iterator_type end = array + properties::size::value;

        return range_type(std::move(beg), std::move(end));
    }

    template<class TPredicate>
    inline linq::generate_range<TPredicate>
    from_generator(TPredicate p)
        { return linq::generate_range<TPredicate>(p); }

/* operators */
    template<class TPredicate>
    inline linq::where_builder<TPredicate>
    where(TPredicate p)
        { return linq::where_builder<TPredicate>(p); }

    template<class TPredicate>
    inline linq::select_builder<TPredicate>
    select(TPredicate p)
        { return linq::select_builder<TPredicate>(p); }

    template<class TPredicate>
    inline linq::select_many_builder<TPredicate>
    select_many(TPredicate p)
        { return linq::select_many_builder<TPredicate>(p); }

    template<class TSelectPredicate, class TLessPredicate>
    inline linq::order_by_builder<TSelectPredicate, TLessPredicate>
    order_by(TSelectPredicate sp, TLessPredicate lp)
        { return linq::order_by_builder<TSelectPredicate, TLessPredicate>(std::move(sp), std::move(lp)); }

    template<class TSelectPredicate>
    inline linq::order_by_builder<TSelectPredicate, linq::op_less_default>
    order_by(TSelectPredicate sp)
        { return linq::order_by_builder<TSelectPredicate, linq::op_less_default>(std::move(sp), linq::op_less_default()); }

    inline linq::order_by_builder<linq::op_select_default, linq::op_less_default>
    order_by()
        { return linq::order_by_builder<linq::op_select_default, linq::op_less_default>(linq::op_select_default(), linq::op_less_default()); }

    inline linq::distinct_builder
    distinct()
        { return linq::distinct_builder(); }

    template<class T>
    inline linq::default_if_empty_builder<T>
    default_if_empty(T t)
        { return linq::default_if_empty_builder<T>(t); }

/* result generators */
    inline linq::sum_builder
    sum()
        { return linq::sum_builder(); }

    inline linq::any_builder
    any()
        { return linq::any_builder(); }

    inline linq::min_builder
    min()
        { return linq::min_builder(); }

    inline linq::max_builder
    max()
        { return linq::max_builder(); }

    template<class T>
    inline linq::contains_builder<T, linq::op_compare_default>
    contains(T t)
        { return linq::contains_builder<T, linq::op_compare_default>(t, linq::op_compare_default()); }

    template<class T, class TPredicate>
    inline linq::contains_builder<T, TPredicate>
    contains(T t, TPredicate p)
        { return linq::contains_builder<T, TPredicate>(t, p); }

    inline linq::single_or_default_builder
    single_or_default()
        { return linq::single_or_default_builder(); }

    inline linq::first_builder
    first()
        { return linq::first_builder(); }

    inline linq::first_or_default_builder
    first_or_default()
        { return linq::first_or_default_builder(); }

    inline linq::last_builder
    last()
        { return linq::last_builder(); }

    inline linq::last_or_default_builder
    last_or_default()
        { return linq::last_or_default_builder(); }

    inline linq::to_vector_builder
    to_vector(size_t capacity = 16)
        { return linq::to_vector_builder(capacity); }

    inline linq::to_list_builder
    to_list()
        { return linq::to_list_builder(); }

    template<class TKeyPredicate, class TValuePredicate>
    inline linq::to_map_builder<TKeyPredicate, TValuePredicate>
    to_map(TKeyPredicate k, TValuePredicate v)
        { return linq::to_map_builder<TKeyPredicate, TValuePredicate>(k, v); }

    template<class TKeyPredicate>
    inline linq::to_map_builder<TKeyPredicate, linq::op_select_default>
    to_map(TKeyPredicate k)
        { return linq::to_map_builder<TKeyPredicate, linq::op_select_default>(k, linq::op_select_default()); }

    inline linq::to_map_builder<linq::op_auto_select_key, linq::op_auto_select_value>
    to_map()
        { return linq::to_map_builder<linq::op_auto_select_key, linq::op_auto_select_value>(linq::op_auto_select_key(), linq::op_auto_select_value()); }

    inline linq::container_builder
    to_container()
        { return linq::container_builder(); }

    template<class TKeyPredicate, class TValuePredicate>
    inline linq::to_lookup_builder<TKeyPredicate, TValuePredicate>
    to_lookup(TKeyPredicate k, TValuePredicate v)
        { return linq::to_lookup_builder<TKeyPredicate, TValuePredicate>(k, v); }

    template<class TKeyPredicate>
    inline linq::to_lookup_builder<TKeyPredicate, linq::op_select_default>
    to_lookup(TKeyPredicate k)
        { return linq::to_lookup_builder<TKeyPredicate, linq::op_select_default>(k, linq::op_select_default()); }

    inline linq::to_lookup_builder<linq::op_auto_select_key, linq::op_auto_select_value>
    to_lookup()
        { return linq::to_lookup_builder<linq::op_auto_select_key, linq::op_auto_select_value>(linq::op_auto_select_key(), linq::op_auto_select_value()); }

    template<class TPredicate>
    inline linq::for_each_builder<TPredicate>
    for_each(TPredicate p)
        { return linq::for_each_builder<TPredicate>(p); }

}