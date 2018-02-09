#pragma once

#include <set>
#include <map>
#include <list>
#include <vector>

#include "Nullable.h"
#include "Exception.h"
#include "MetaProgramming.h"

// #define LINQ_DEBUG

#ifdef LINQ_DEBUG
    #include "Misc.h"
    #define LINQ_TYPE_NAME()    utl::TypeHelper<decltype(*this)>::name()
    #define LINQ_CTOR()         do { std::cout << "CTOR(this=" << this << ")" << LINQ_TYPE_NAME() << std::endl; } while(0)
    #define LINQ_COPY_CTOR()    do { std::cout << "COPY(this=" << this << ")" << LINQ_TYPE_NAME() << std::endl; } while(0)
    #define LINQ_MOVE_CTOR()    do { std::cout << "MOVE(this=" << this << ")" << LINQ_TYPE_NAME() << std::endl; } while(0)
    #define LINQ_DTOR()         do { std::cout << "DTOR(this=" << this << ")" << LINQ_TYPE_NAME() << std::endl; } while(0)
#else
    #define LINQ_TYPE_NAME()    while(0)
    #define LINQ_CTOR()         while(0)
    #define LINQ_COPY_CTOR()    while(0)
    #define LINQ_MOVE_CTOR()    while(0)
    #define LINQ_DTOR()         while(0)
#endif

namespace linq
{
    namespace __impl
    {
        struct tag_range { };
        struct tag_builder { };

        /* meta programming **********************************************************************/
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
        using mp_range_value_type = typename utl::mp_remove_ref<T>::value_type;

        /* helper types **************************************************************************/
        template<class T>
        struct wrapper
        {
            using value_type = T;

            value_type value;

            inline value_type operator*() const
                { return value; }

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

            template<class X>
            inline wrapper(X&& v) :
                value(std::forward<X>(v))
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
                { return *value; }

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

        template<class T, class TPredicate>
        struct op_wrapper_less
        {
            using predicate_type    = TPredicate;
            using value_type        = wrapper<T>;

            predicate_type predicate;

            inline bool operator()(const value_type& l, const value_type& r)
                { return predicate(*l, *r); }

            inline op_wrapper_less(const predicate_type& lp) :
                predicate(lp)
                { }

            inline op_wrapper_less(const op_wrapper_less& other) :
                predicate(other.predicate)
                { }

            inline op_wrapper_less(op_wrapper_less&& other) :
                predicate(std::move(other.predicate))
                { }
        };

        template<class TRange>
        struct range_wrapper
        {
            using range_type = TRange;
            using this_type  = range_wrapper<range_type>;
            using value_type = mp_range_value_type<range_type>;

            range_type range;

            inline value_type& front()
                { return range.front(); }

            inline bool next()
                { return range.next(); }

            template<class TBuilder>
            inline auto operator >> (TBuilder&& builder) &
                { return builder.build(range); }

            template<class TBuilder>
            inline auto operator >> (TBuilder&& builder) &&
                { return builder.build(std::move(range)); }

            template<class... Args>
            range_wrapper(Args&&... args) :
                range(std::forward<Args>(args)...)
                { }

            range_wrapper(const this_type& other) :
                range(other.range)
                { }

            range_wrapper(this_type&& other) :
                range(std::move(other.range))
                { }
        };

        template<class TKey, class TValue>
        struct lookup
        {
        public:
            using key_type              = TKey;
            using clean_key_type        = utl::mp_remove_ref<key_type>;
            using value_type            = TValue;
            using this_type             = lookup<key_type, value_type>;
            using wrapped_key_type      = wrapper<key_type>;
            using wrapped_value_type    = wrapper<value_type>;
            using keys_value_type       = std::pair<wrapped_key_type, size_t>;
            using keys_type             = std::vector<keys_value_type>;
            using values_type           = std::vector<wrapped_value_type>;
            using range_indices_type    = std::pair<size_t, size_t>;

        private:
            struct lookup_range;
            struct lookup_key_value_range;

            using lookup_range_wrapper           = range_wrapper<lookup_range>;
            using lookup_key_value_range_wrapper = range_wrapper<lookup_key_value_range>;

        public:
            using range_type = lookup_key_value_range_wrapper;

        private:
            struct lookup_range : public tag_range
            {
                using value_type = lookup::value_type;

                enum class State
                {
                    Initialize,
                    Iterating,
                    Finished,
                };

                const values_type&  values;
                size_t              current;
                size_t              end;
                State               state;

                inline value_type& front()
                {
                    assert(state == State::Iterating);
                    assert(current < end);
                    return *values[current];
                }

                inline bool next()
                {
                    switch (state)
                    {
                        case State::Iterating:
                        {
                            ++current;
                        }

                        case State::Initialize:
                        {
                            auto hasElements = (current < end);
                            state = (hasElements ? State::Iterating : State::Finished);
                            return hasElements;
                        }

                        default:
                        {
                            return false;
                        }
                    }
                }

                inline lookup_range(const values_type& v, size_t c, size_t e) :
                    values  (v),
                    current (c),
                    end     (e),
                    state   (State::Initialize)
                    { LINQ_CTOR(); }

                inline lookup_range(const lookup_range& other) :
                    values  (other.values),
                    current (other.current),
                    end     (other.end),
                    state   (other.state)
                    { LINQ_COPY_CTOR(); }

                inline lookup_range(lookup_range&& other) :
                    values  (std::move(other.values)),
                    current (std::move(other.current)),
                    end     (std::move(other.end)),
                    state   (std::move(other.state))
                    { LINQ_MOVE_CTOR(); }

                inline ~lookup_range()
                    { LINQ_DTOR(); }
            };

            struct lookup_key_value_range : public tag_range
            {
                using value_type    = std::pair<key_type, lookup_range_wrapper>;
                using iterator_type = typename keys_type::const_iterator;

                lookup              container;
                bool                initialized;
                range_indices_type  range_indices;
                iterator_type       current;
                iterator_type       end;

                inline value_type front()
                {
                    assert(initialized);
                    assert(current != end);
                    return value_type(
                        *current->first,
                        std::move(container.createRange(range_indices)));
                }

                inline bool next()
                {
                    if (!initialized)
                    {
                        initialized = true;
                        current     = container._keys.begin();
                        end         = container._keys.end();
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
                    range_indices = container.findKey(*current->first);
                    return true;
                }

                template<class C>
                lookup_key_value_range(C&& c) :
                    container   (std::forward<C>(c)),
                    initialized (false)
                    { LINQ_CTOR(); }

                lookup_key_value_range(const lookup_key_value_range& other) :
                    container       (other.container),
                    initialized     (other.initialized),
                    range_indices   (other.range_indices),
                    current         (other.current),
                    end             (other.end)
                    { LINQ_COPY_CTOR(); }

                lookup_key_value_range(lookup_key_value_range&& other) :
                    container       (std::move(other).container),
                    initialized     (std::move(other).initialized),
                    range_indices   (std::move(other).range_indices),
                    current         (std::move(other).current),
                    end             (std::move(other).end)
                    { LINQ_MOVE_CTOR(); }

                ~lookup_key_value_range()
                    { LINQ_DTOR(); }
            };

            struct op_compare_keys
            {
                bool operator()(
                    const keys_value_type& l,
                    const keys_value_type& r)
                    { return *l.first < *r.first; }
            };

        private:
            keys_type   _keys;
            values_type _values;

            inline range_indices_type findKey(const clean_key_type& key)
            {
                if (_values.empty())
                    return std::make_pair<size_t, size_t>(0, 0);

                auto it = std::lower_bound(
                            _keys.begin(),
                            _keys.end(),
                            typename keys_type::value_type (const_cast<clean_key_type&>(key), 0),
                            op_compare_keys());
                if (    it == _keys.end()
                    || *it->first != const_cast<clean_key_type&>(key))
                    return std::make_pair<size_t, size_t>(0, 0);

                auto next = it + 1;
                range_indices_type ret;
                ret.first  = it->second;
                ret.second = next == _keys.end()
                    ? _values.size()
                    : next->second;
                return ret;
            }

            inline lookup_range_wrapper createRange(const range_indices_type& indices)
                { return lookup_range_wrapper(_values, indices.first, indices.second); }

        public:
            inline lookup_range_wrapper operator[](const clean_key_type& key)
                { return createRange(findKey(key)); }

            template<class TBuilder>
            inline auto operator >> (TBuilder&& builder) &
                { return builder.build(std::move(lookup_key_value_range(*this))); }

            template<class TBuilder>
            inline auto operator >> (TBuilder&& builder) &&
                { return builder.build(std::move(lookup_key_value_range(std::move(*this)))); }

        private:
            inline lookup(keys_type&& k, values_type&& v) :
                _keys   (k),
                _values (v)
                { LINQ_CTOR(); }

        public:
            inline lookup()
                { LINQ_CTOR(); }

            inline lookup(const lookup& other) :
                _keys   (other._keys),
                _values (other._values)
                { LINQ_COPY_CTOR(); }

            inline lookup(lookup&& other) :
                _keys   (std::move(other)._keys),
                _values (std::move(other)._values)
                { LINQ_MOVE_CTOR(); }

            inline ~lookup()
                { LINQ_DTOR(); }

        public:
            template<class TRange, class TKeyPredicate, class TValuePredicate>
            static inline auto build(TRange& range, TKeyPredicate& kp, TValuePredicate& vp)
            {
                keys_type   k;
                values_type v;

                size_t index = 0;
                while (range.next())
                {
                    k.emplace_back(kp(range.front()), index);
                    v.emplace_back(vp(range.front()));
                    ++index;
                }
                if (v.empty())
                    return lookup();

                std::sort(k.begin(), k.end(), op_compare_keys());

                keys_type keys;
                values_type values;

                keys.reserve  (k.size());
                values.reserve(k.size());

                auto it  = k.begin();
                auto end = k.end();
                index = 0;

                if (it != end)
                {
                    keys.emplace_back(it->first, index);
                    values.push_back(v.at(it->second));
                }

                auto prev = it;
                ++it;
                ++index;
                while (it != end)
                {
                    values.push_back(v.at(it->second));
                    if (*prev->first < *it->first)
                        keys.push_back(std::make_pair(it->first, index));
                    prev = it;
                    ++it;
                    ++index;
                }

                return lookup(std::move(keys), std::move(values));
            }
        };

        /* ranges ********************************************************************************/
        template<class TIterator>
        struct iterator_range : public tag_range
        {
            using iterator_type = TIterator;
            using this_type     = iterator_range<iterator_type>;
            using value_type    = decltype(*std::declval<iterator_type>());

            bool            initialized;
            iterator_type   current;
            iterator_type   end;

            inline value_type& front()
            {
                assert(initialized);
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

            inline iterator_range(iterator_type b, iterator_type e) :
                initialized (false),
                current     (b),
                end         (e)
                { LINQ_CTOR(); }

            inline iterator_range(const this_type& other) :
                initialized (other.initialized),
                current     (other.current),
                end         (other.end)
                { LINQ_COPY_CTOR(); }

            inline iterator_range(this_type&& other) :
                initialized (std::move(other).initialized),
                current     (std::move(other).current),
                end         (std::move(other).end)
                { LINQ_MOVE_CTOR(); }

            inline ~iterator_range()
                { LINQ_DTOR(); }
        };

        template<class TIterator>
        using iterator_range_wrapper = range_wrapper<iterator_range<TIterator>>;

        template<class TContainer>
        struct container_range : public tag_range
        {
            using container_type    = TContainer;
            using this_type         = container_range<container_type>;
            using iterator_type     = decltype(std::begin(std::declval<container_type>()));
            using value_type        = decltype(*std::declval<iterator_type>());

            TContainer      container;
            bool            initialized;
            iterator_type   current;

            inline value_type& front()
            {
                assert(initialized);
                assert(current != std::end(container));
                return *current;
            }

            inline bool next()
            {
                if (!initialized)
                {
                    initialized = true;
                    current = std::begin(container);
                }
                else if (current != std::end(container))
                {
                    ++current;
                }
                return (current != std::end(container));
            }

            inline container_range(container_type& c) noexcept :
                container   (c),
                initialized (false)
                { LINQ_CTOR(); }

            inline container_range(const this_type& other) noexcept :
                container   (other.container),
                initialized (other.initialized),
                current     (other.current)
                { LINQ_COPY_CTOR(); }

            inline container_range(this_type&& other) noexcept :
                container   (std::move(other).container),
                initialized (std::move(other).initialized),
                current     (std::move(other).current)
                { LINQ_MOVE_CTOR(); }

            inline ~container_range()
                { LINQ_DTOR(); }
        };

        template<class TContainer>
        using container_range_wrapper = range_wrapper<container_range<TContainer>>;

        template<class TPredicate>
        struct generator_range : public tag_range
        {
            using predicate_type        = TPredicate;
            using this_type             = generator_range<predicate_type>;
            using predicate_value_type  = decltype(std::declval<predicate_type>()());
            using value_type            = decltype(*std::declval<predicate_value_type>());

            predicate_type          predicate;
            predicate_value_type    value;

            inline value_type& front()
            {
                assert(static_cast<bool>(value));
                return *value;
            }

            inline bool next()
            {
                value = predicate();
                return static_cast<bool>(value);
            }

            inline generator_range(predicate_type p) :
                predicate   (p)
                { LINQ_CTOR(); }

            inline generator_range(const this_type& other) :
                predicate   (other.predicate),
                value       (other.value)
                { LINQ_COPY_CTOR(); }

            inline generator_range(this_type&& other) :
                predicate   (std::move(other).predicate),
                value       (std::move(other).value)
                { LINQ_MOVE_CTOR(); }

            inline ~generator_range()
                { LINQ_DTOR(); }
        };

        template<class TPredicate>
        using generator_range_wrapper = range_wrapper<generator_range<TPredicate>>;

        template<class TRange, class TPredicate>
        struct where_range : public tag_range
        {
            using range_type        = TRange;
            using predicate_type    = TPredicate;
            using this_type         = where_range<range_type, predicate_type>;
            using value_type        = mp_range_value_type<range_type>;

            range_type      range;
            predicate_type  predicate;

            inline value_type& front()
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

            template<class R, class P>
            inline where_range(R&& r, P&& p) :
                range       (std::forward<R>(r)),
                predicate   (std::forward<P>(p))
                { LINQ_CTOR(); }

            inline where_range(const this_type& other) :
                range       (other.range),
                predicate   (other.predicate)
                { LINQ_COPY_CTOR(); }

            inline where_range(this_type&& other) :
                range       (std::move(other).range),
                predicate   (std::move(other).predicate)
                { LINQ_MOVE_CTOR(); }

            inline ~where_range()
                { LINQ_DTOR(); }
        };

        template<class TRange, class TPredicate>
        using where_range_wrapper = range_wrapper<where_range<TRange, TPredicate>>;

        template<class TRange, class TPredicate>
        struct select_range : public tag_range
        {
            using range_type        = TRange;
            using predicate_type    = TPredicate;
            using this_type         = select_range<range_type, predicate_type>;
            using range_value_type  = mp_range_value_type<range_type>;
            using value_type        = decltype(std::declval<predicate_type>()(std::declval<range_value_type>()));
            using cache_type        = utl::Nullable<value_type>;

            predicate_type  predicate;
            range_type      range;
            cache_type      cache;

            inline value_type& front()
            {
                assert(static_cast<bool>(cache));
                return *cache;
            }

            inline bool next()
            {
                if (range.next())
                {
                    cache = predicate(range.front());
                    return true;
                }
                cache.reset();
                return false;
            }

            template<class R, class P>
            inline select_range(R&& r, P&& p) :
                range       (std::forward<R>(r)),
                predicate   (std::forward<P>(p))
                { LINQ_CTOR(); }

            inline select_range(const this_type& other) :
                range       (other.range),
                predicate   (other.predicate)
                { LINQ_COPY_CTOR(); }

            inline select_range(this_type&& other) :
                range       (std::move(other).range),
                predicate   (std::move(other).predicate)
                { LINQ_MOVE_CTOR(); }

            inline ~select_range()
                { LINQ_DTOR(); }
        };

        template<class TRange, class TPredicate>
        using select_range_wrapper = range_wrapper<select_range<TRange, TPredicate>>;

        template<class TRange, class TPredicate>
        struct select_many_range : public tag_range
        {
            template<class T>
            struct __impl_make_inner_range
            {
                using iterator_type = decltype(std::begin(std::declval<T>()));
                using type          = iterator_range<iterator_type>;
            };

            template<class T>
            using mp_make_inner_range = typename __impl_make_inner_range<T>::type;

            using range_type                = TRange;
            using predicate_type            = TPredicate;
            using this_type                 = where_range<range_type, predicate_type>;
            using range_value_type          = mp_range_value_type<range_type>;
            using predicate_return_type     = decltype(std::declval<predicate_type>()(std::declval<range_value_type>()));
            using inner_range_type          = utl::mp_eval_if<
                                                std::is_base_of<tag_range, predicate_return_type>,
                                                predicate_return_type,
                                                mp_make_inner_range, predicate_return_type>;
            using value_type                = mp_range_value_type<inner_range_type>;
            using inner_range_cache_type    = utl::Nullable<inner_range_type>;

            predicate_type          predicate;
            range_type              range;
            inner_range_cache_type  inner_range;

            template<class T>
            inline typename std::enable_if<std::is_base_of<tag_range, T>::value>::type
            build_inner_range(T value)
                { inner_range = value; }

            template<class T>
            inline typename std::enable_if<!std::is_base_of<tag_range, T>::value>::type
            build_inner_range(T value)
                { inner_range = inner_range_type(std::begin(value), std::end(value)); }

            inline value_type& front()
            {
                assert(inner_range);
                return inner_range->front();
            }

            inline bool next()
            {
                if (inner_range && inner_range->next())
                    return true;
                while (range.next())
                {
                    inner_range.reset();
                    build_inner_range<predicate_return_type>(predicate(range.front()));
                    if (inner_range && inner_range->next())
                        return true;
                }
                inner_range.reset();
                return false;
            }

            template<class R, class P>
            inline select_many_range(R&& r, P&& p) :
                range       (std::forward<R>(r)),
                predicate   (std::forward<P>(p))
                { LINQ_CTOR(); }

            inline select_many_range(const this_type& other) :
                range       (other.range),
                predicate   (other.predicate),
                inner_range (other.inner_range)
                { LINQ_COPY_CTOR(); }

            inline select_many_range(this_type&& other) :
                range       (std::move(other).range),
                predicate   (std::move(other).predicate),
                inner_range (std::move(other).inner_range)
                { LINQ_MOVE_CTOR(); }

            inline ~select_many_range()
                { LINQ_DTOR(); }
        };

        template<class TRange, class TPredicate>
        using select_many_range_wrapper = range_wrapper<select_many_range<TRange, TPredicate>>;

        template<class TRange, class TSelectPredicate, class TLessPredicate>
        struct order_by_range : public tag_range
        {
            using range_type            = TRange;
            using select_predicate_type = TSelectPredicate;
            using less_predicate_type   = TLessPredicate;
            using this_type             = order_by_range<range_type, select_predicate_type, less_predicate_type>;
            using value_type            = mp_range_value_type<range_type>;
            using wrapped_value_type    = wrapper<value_type>;
            using vector_type           = std::vector<wrapped_value_type>;

            range_type              range;
            select_predicate_type   select_predicate;
            less_predicate_type     less_predicate;
            ssize_t                 current;
            vector_type             values;

            inline value_type& front()
            {
                assert(current >= 0 && current < values.size());
                return *values.at(current);
            }

            inline bool next()
            {
                if (current < 0)
                {
                    values.clear();
                    while (range.next())
                        values.emplace_back(range.front());

                    if (values.empty())
                        return false;

                    std::sort(
                    values.begin(),
                    values.end(),
                    [this](wrapped_value_type& l, wrapped_value_type& r) {
                        return this->less_predicate(
                            this->select_predicate(*l),
                            this->select_predicate(*r));
                    });
                    
                    current = 0;
                    return true;
                }

                if (current < static_cast<ssize_t>(values.size()))
                    ++current;
                return (current < static_cast<ssize_t>(values.size()));
            }

            template<class R, class SP, class LP>
            inline order_by_range(R&& r, SP&& sp, LP&& lp) :
                range           (std::forward<R>(r)),
                select_predicate(std::forward<SP>(sp)),
                less_predicate  (std::forward<LP>(lp)),
                current         (-1)
                { LINQ_CTOR(); }

            inline order_by_range(const this_type& other) :
                range           (other.range),
                select_predicate(other.select_predicate),
                less_predicate  (other.less_predicate),
                current         (other.current)
                { LINQ_COPY_CTOR(); }

            inline order_by_range(this_type&& other) :
                range           (std::move(other).range),
                select_predicate(std::move(other).select_predicate),
                less_predicate  (std::move(other).less_predicate),
                current         (std::move(other).current)
                { LINQ_MOVE_CTOR(); }

            inline ~order_by_range()
                { LINQ_DTOR(); }
        };

        template<class TRange, class TSelectPredicate, class TLessPredicate>
        using order_by_range_wrapper = range_wrapper<order_by_range<TRange, TSelectPredicate, TLessPredicate>>;

        template<class TRange, class TLessPredicate>
        struct distinct_range : public tag_range
        {
            using range_type            = TRange;
            using less_predicate_type   = TLessPredicate;
            using this_type             = distinct_range<range_type, less_predicate_type>;
            using value_type            = mp_range_value_type<range_type>;
            using set_value_type        = wrapper<value_type>;
            using set_less_type         = op_wrapper_less<value_type, less_predicate_type>;
            using set_type              = std::set<set_value_type, set_less_type>;

            range_type  range;
            set_type    set;

            inline value_type& front()
                { return range.front(); }

            inline bool next()
            {
                while(range.next())
                {
                    if (set.emplace(range.front()).second)
                        return true;
                }
                return false;
            }

            template<class R, class LP>
            inline distinct_range(R&& r, LP&& lp) :
                range   (std::forward<R>(r)),
                set     (std::forward<LP>(lp))
                { LINQ_CTOR(); }

            inline distinct_range(const this_type& other) :
                range   (other.range),
                set     (other.set)
                { LINQ_COPY_CTOR(); }

            inline distinct_range(this_type&& other) :
                range   (std::move(other).range),
                set     (std::move(other).set)
                { LINQ_MOVE_CTOR(); }

            inline ~distinct_range()
                { LINQ_DTOR(); }
        };

        template<class TRange, class TLessPredicate>
        using distinct_range_wrapper = range_wrapper<distinct_range<TRange, TLessPredicate>>;

        /* builder *******************************************************************************/
        template<template<class> class TOuterRange>
        struct builder : public tag_builder
        {
            template<class R>
            using outer_range_type  = TOuterRange<R>;
            using this_type         = builder<outer_range_type>;

            template<class TRange>
            inline auto build(TRange&& range)
            {
                // CAUTION: we want no reference to a range here, because the passed range may be destroyed before used in outer_range_type
                using range_type = utl::mp_remove_ref<TRange>;
                return outer_range_type<range_type>(std::forward<TRange>(range));
            }

            inline builder()
                { LINQ_CTOR(); }

            inline builder(this_type&& other)
                { LINQ_MOVE_CTOR(); }

            inline builder(const this_type&) = delete;

            inline ~builder()
                { LINQ_DTOR(); }
        };

        template<class TPredicate, template<class, class> class TOuterRange>
        struct predicate_builder : public tag_builder
        {
            template<class R, class P>
            using outer_range_type  = TOuterRange<R, P>;
            using predicate_type    = TPredicate;
            using this_type         = predicate_builder<predicate_type, outer_range_type>;

            predicate_type predicate;

            template<class TRange>
            inline auto build(TRange&& range)
            {
                // CAUTION: we want no reference to a range here, because the passed range may be destroyed before used in outer_range_type
                using range_type = utl::mp_remove_ref<TRange>;
                return outer_range_type<range_type, predicate_type>(std::forward<TRange>(range), std::move(predicate));
            }

            inline predicate_builder(const predicate_type& p) :
                predicate(p)
                { LINQ_CTOR(); }

            inline predicate_builder(this_type&& other) :
                predicate(std::move(other).predicate)
                { LINQ_MOVE_CTOR(); }

            inline predicate_builder(const this_type&) = delete;

            inline ~predicate_builder()
                { LINQ_DTOR(); }
        };

        template<class TPredicate0, class TPredicate1, template<class, class, class> class TOuterRange>
        struct dual_predicate_builder : public tag_builder
        {
            template<class R, class P0, class P1>
            using outer_range_type  = TOuterRange<R, P0, P1>;
            using predicate_0_type  = TPredicate0;
            using predicate_1_type  = TPredicate1;
            using this_type         = dual_predicate_builder<predicate_0_type, predicate_1_type, outer_range_type>;

            predicate_0_type predicate0;
            predicate_1_type predicate1;

            template<class TRange>
            inline auto build(TRange&& range)
            {
                // CAUTION: we want no reference to a range here, because the passed range may be destroyed before used in outer_range_type
                using range_type = utl::mp_remove_ref<TRange>;
                return outer_range_type<range_type, predicate_0_type, predicate_1_type>(std::forward<range_type>(range), std::move(predicate0), std::move(predicate1));
            }

            inline dual_predicate_builder(const predicate_0_type& p0, const predicate_1_type& p1) :
                predicate0(p0),
                predicate1(p1)
                { LINQ_CTOR(); }

            inline dual_predicate_builder(this_type&& other) :
                predicate0(std::move(other).predicate0),
                predicate1(std::move(other).predicate1)
                { LINQ_MOVE_CTOR(); }

            inline dual_predicate_builder(const this_type&) = delete;

            inline ~dual_predicate_builder()
                { LINQ_DTOR(); }
        };

        struct count_builder : public tag_builder
        {
            template<class TRange>
            inline auto build(TRange&& range)
            {
                size_t ret = 0;
                while (range.next())
                    ++ret;
                return ret;
            }
        };

        struct sum_builder : public tag_builder
        {
            template<class TRange>
            inline auto build(TRange&& range)
            {
                using value_type  = mp_range_value_type<TRange>;
                using return_type = utl::mp_clean_type<value_type>;

                return_type sum = return_type();
                while (range.next())
                    sum += range.front();
                return sum;
            }
        };

        struct min_builder : public tag_builder
        {
            template<class TRange>
            inline auto build(TRange&& range)
            {
                using value_type  = mp_range_value_type<TRange>;
                using return_type = utl::mp_clean_type<value_type>;

                return_type ret = std::numeric_limits<return_type>::max();
                while (range.next())
                {
                    if (ret > range.front())
                        ret = range.front();
                }
                return ret;
            }
        };

        struct max_builder : public tag_builder
        {
            template<class TRange>
            inline auto build(TRange&& range)
            {
                using value_type  = mp_range_value_type<TRange>;
                using return_type = utl::mp_clean_type<value_type>;

                return_type ret = std::numeric_limits<return_type>::min();
                while (range.next())
                {
                    if (ret < range.front())
                        ret = range.front();
                }
                return ret;
            }
        };

        struct any_builder : public tag_builder
        {
            template<class TRange>
            inline auto build(TRange&& range)
                { return range.next(); }
        };

        template <class T, class TPredicate>
        struct contains_builder : public tag_builder
        {
            using comperator_type  = T;
            using predicate_type   = TPredicate;
            using this_type        = contains_builder<comperator_type, predicate_type>;

            comperator_type comperator;
            predicate_type  predicate;

            template<class TRange>
            inline auto build(TRange&& range)
            {
                while(range.next())
                {
                    if (predicate(comperator, range.front()))
                        return true;
                }
                return false;
            }

            inline contains_builder(comperator_type&& c, predicate_type&& p) :
                comperator(c),
                predicate (p)
                { }
        };

        struct single_builder : public tag_builder
        {
            template<class TRange>
            inline auto build(TRange&& range)
            {
                if (!range.next())
                    throw utl::Exception("range is empty");
                auto ret = std::move(range.front());
                if (range.next())
                    throw utl::Exception("range contains more than one value");
                return ret;
            }
        };

        struct single_or_default_builder : public tag_builder
        {
            template<class TRange>
            inline auto build(TRange&& range)
            {
                using range_value_type = mp_range_value_type<TRange>;
                using value_type       = utl::mp_remove_ref<range_value_type>;
                if (!range.next())
                    return value_type();
                auto ret = std::move(range.front());
                if (range.next())
                    return value_type();
                return ret;
            }
        };

        struct first_builder : public tag_builder
        {
            template<class TRange>
            inline auto build(TRange&& range)
            {
                if (!range.next())
                    throw utl::Exception("range is empty");
                return std::move(range.front());
            }
        };

        struct first_or_default_builder : public tag_builder
        {
            template<class TRange>
            inline auto build(TRange&& range)
            {
                using range_value_type = mp_range_value_type<TRange>;
                using value_type       = utl::mp_remove_ref<range_value_type>;
                if (!range.next())
                    return value_type();
                return std::move(range.front());
            }
        };

        struct last_builder_base
        {
            template<class T>
            struct cache
            {
                std::unique_ptr<T> value;

                inline cache& operator=(T& t)
                    { value.reset(new T(std::move(t))); return *this; }

                inline T& operator*()
                    { return *value; }

                inline operator bool()
                    { return static_cast<bool>(value); }

                inline cache()
                    { }
            };

            template<class T>
            struct cache<T&>
            {
                T* value;

                inline cache& operator=(T& t)
                    { value = &t; return *this; }

                inline T& operator*()
                    { return *value; }

                inline operator bool()
                    { return static_cast<bool>(value); }

                inline cache() :
                    value(nullptr)
                    { }
            };
        };

        struct last_builder : public tag_builder, public last_builder_base
        {
            template<class TRange>
            inline auto build(TRange&& range)
            {
                using value_type = mp_range_value_type<TRange>;
                using cache_type = cache<value_type>;

                cache_type tmp;
                while(range.next())
                    tmp = range.front();
                if (!static_cast<bool>(tmp))
                    throw utl::Exception("range is empty");
                return std::move(*tmp);
            }
        };

        struct last_or_default_builder : public tag_builder, public last_builder_base
        {
            template<class TRange>
            inline auto build(TRange&& range)
            {
                using range_value_type  = mp_range_value_type<TRange>;
                using value_type        = utl::mp_remove_ref<range_value_type>;
                using cache_type        = cache<value_type>;

                cache_type tmp;
                while(range.next())
                    tmp = range.front();
                if (!static_cast<bool>(tmp))
                    return value_type();
                return std::move(*tmp);
            }
        };

        struct to_vector_builder : public tag_builder
        {
            size_t capacity;

            template<class TRange>
            inline auto build(TRange&& range)
            {
                using range_value_type = mp_range_value_type<TRange>;
                using value_type       = utl::mp_remove_ref<range_value_type>;
                using vector_type      = std::vector<value_type>;

                vector_type ret;
                ret.reserve(capacity);
                while (range.next())
                    ret.emplace_back(std::move(range.front()));
                return ret;
            }

            inline to_vector_builder(size_t cap = 16) :
                capacity(cap)
                { }
        };

        struct to_list_builder : public tag_builder
        {
            template<class TRange>
            inline auto build(TRange&& range)
            {
                using range_value_type = mp_range_value_type<TRange>;
                using value_type       = utl::mp_remove_ref<range_value_type>;
                using list_type        = std::list<value_type>;

                list_type ret;
                while (range.next())
                    ret.emplace_back(std::move(range.front()));
                return ret;
            }
        };

        template<class TKeyPredicate, class TValuePredicate>
        struct to_map_builder : public tag_builder
        {
            using key_predicate_type    = TKeyPredicate;
            using value_predicate_type  = TValuePredicate;
            using this_type             = to_map_builder<key_predicate_type, value_predicate_type>;

            key_predicate_type      key_predicate;
            value_predicate_type    value_predicate;

            template<class TRange>
            inline auto build(TRange&& range)
            {
                using range_type        = TRange;
                using range_value_type  = mp_range_value_type<range_type>;
                using key_type          = decltype(std::declval<key_predicate_type>()(std::declval<range_value_type&>()));
                using value_type        = decltype(std::declval<value_predicate_type>()(std::declval<range_value_type&>()));
                using map_type          = std::map<utl::mp_remove_ref<key_type>, value_type>;

                map_type map;
                while (range.next())
                {
                    auto ret = map.emplace(
                        key_predicate(range.front()),
                        value_predicate(range.front()));
                    if (!ret.second)
                        throw utl::Exception("duplicate key in range");
                }
                return map;
            }

            inline to_map_builder(const key_predicate_type& kp, const value_predicate_type& vp) :
                key_predicate   (kp),
                value_predicate (vp)
                { LINQ_CTOR(); }
        };

        template<class TPredicate>
        struct for_each_builder : public tag_builder
        {
            using predicate_type    = TPredicate;
            using this_type         = for_each_builder<predicate_type>;

            predicate_type predicate;

            template<class TRange>
            inline auto build(TRange&& range)
            {
                while(range.next())
                    predicate(range.front());
            }

            inline for_each_builder(const predicate_type& p) :
                predicate(p)
                { }
        };

        template<class TKeyPredicate, class TValuePredicate>
        struct to_lookup_builder : public tag_builder
        {
            using key_predicate_type    = TKeyPredicate;
            using value_predicate_type  = TValuePredicate;
            using this_type             = to_lookup_builder<key_predicate_type, value_predicate_type>;

            key_predicate_type      key_predicate;
            value_predicate_type    value_predicate;

            template<class TRange>
            inline auto build(TRange&& range)
            {
                using range_type        = TRange;
                using range_value_type  = mp_range_value_type<range_type>;
                using key_type          = decltype(std::declval<key_predicate_type>()(std::declval<range_value_type>()));
                using value_type        = decltype(std::declval<value_predicate_type>()(std::declval<range_value_type>()));
                using lookup_type       = lookup<key_type, value_type>;
                return lookup_type::build(range, key_predicate, value_predicate);
            }

            inline to_lookup_builder(const key_predicate_type& kp, const value_predicate_type& vp) :
                key_predicate   (kp),
                value_predicate (vp)
                { LINQ_CTOR(); }
        };
    }

    /* default operations ********************************************************************/
    struct op_select_default
    {
        template<class T>
        inline T operator()(T t)
            { return t; }
    };

    struct op_less_default
    {
        template<class L, class R>
        inline bool operator()(L&& l, R&& r) const
            { return (l < r); }
    };

    struct op_compare_default
    {
        template<class L, class R>
        inline bool operator()(L&& l, R&& r) const
            { return (l == r); }
    };

    struct op_select_key_default
    {
        template<class TKey, class TValue>
        inline auto operator()(std::pair<TKey, TValue>& p)
            { return p.first; }

        template<class TKey, class TValue>
        inline auto operator()(std::tuple<TKey, TValue>& t)
            { return std::get<0>(t); }

        template<class T>
        inline auto operator()(T& t)
            { return t; }
    };

    struct op_select_value_default
    {
        template<class TKey, class TValue>
        inline auto operator()(std::pair<TKey, TValue>& p)
            { return p.second; }

        template<class TKey, class TValue>
        inline auto operator()(std::tuple<TKey, TValue>& t)
            { return std::get<1>(t); }

        template<class T>
        inline auto operator()(T&& t)
            { return std::forward<T>(t); }
    };

    /* constructors ******************************************************************************/
    template<class TIterator>
    inline auto from_iterator(TIterator&& beg, TIterator&& end)
        { return __impl::iterator_range_wrapper<TIterator>(std::forward<TIterator>(beg), std::forward<TIterator>(end)); }

    template<class TContainer>
    inline auto from_container(TContainer&& container)
        { return __impl::container_range_wrapper<TContainer>(std::forward<TContainer>(container)); }

    template<class TPredicate>
    inline auto from_generator(TPredicate&& predicate)
        { return __impl::generator_range_wrapper<TPredicate>(std::forward<TPredicate>(predicate)); }

    template<class TArray>
    inline auto from_array(TArray&& array)
    {
        using array_type = typename std::remove_reference<TArray>::type;
        using array_size = typename __impl::mp_array_properties<array_type>::size;
        return from_iterator(&array[0], &array[array_size::value]);
    }

    /* filter ************************************************************************************/
    template<class TPredicate>
    inline auto where(TPredicate&& predicate)
        { return __impl::predicate_builder<TPredicate, __impl::where_range_wrapper>(std::forward<TPredicate>(predicate)); }

    template<class TPredicate>
    inline auto select(TPredicate&& predicate)
        { return __impl::predicate_builder<TPredicate, __impl::select_range_wrapper>(std::forward<TPredicate>(predicate)); }

    template<class TPredicate>
    inline auto select_many(TPredicate&& predicate)
        { return __impl::predicate_builder<TPredicate, __impl::select_many_range_wrapper>(std::forward<TPredicate>(predicate)); }

    template<class TSelectPredicate, class TLessPredicate>
    inline auto order_by(TSelectPredicate&& sp, TLessPredicate&& lp)
        { return __impl::dual_predicate_builder<TSelectPredicate, TLessPredicate, __impl::order_by_range_wrapper>(std::forward<TSelectPredicate>(sp), std::forward<TLessPredicate>(lp)); }

    template<class TSelectPredicate>
    inline auto order_by(TSelectPredicate&& sp)
        { return order_by(std::forward(sp), op_less_default()); }

    inline auto order_by()
        { return order_by(op_select_default(), op_less_default()); }

    template<class TLessPredicate>
    inline auto distinct(TLessPredicate&& lp)
        { return __impl::predicate_builder<TLessPredicate, __impl::distinct_range_wrapper>(std::forward<TLessPredicate>(lp)); }

    inline auto distinct()
        { return distinct(op_less_default()); }

    /* result generators */
    inline auto count()
        { return __impl::count_builder(); }

    inline auto sum()
        { return __impl::sum_builder(); }

    inline auto min()
        { return __impl::min_builder(); }

    inline auto max()
        { return __impl::max_builder(); }

    inline auto any()
        { return __impl::any_builder(); }

    template<class T, class TPredicate>
    inline auto contains(T&& t, TPredicate&& p)
        { return __impl::contains_builder<T, TPredicate>(std::forward<T>(t), std::forward<TPredicate>(p)); }

    template<class T>
    inline auto contains(T&& t)
        { return contains(std::forward<T>(t), op_compare_default()); }

    inline auto single()
        { return __impl::single_builder(); }

    inline auto single_or_default()
        { return __impl::single_or_default_builder(); }

    inline auto first()
        { return __impl::first_builder(); }

    inline auto first_or_default()
        { return __impl::first_or_default_builder(); }

    inline auto last()
        { return __impl::last_builder(); }

    inline auto last_or_default()
        { return __impl::last_or_default_builder(); }

    inline auto to_vector(size_t capacity = 16)
        { return __impl::to_vector_builder(capacity); }

    inline auto to_list()
        { return __impl::to_list_builder(); }

    template<class TKeyPredicate, class TValuePredicate>
    inline auto to_map(TKeyPredicate&& kp, TValuePredicate&& vp)
        { return __impl::to_map_builder<TKeyPredicate, TValuePredicate>(std::forward<TKeyPredicate>(kp), std::forward<TValuePredicate>(vp)); }

    template<class TKeyPredicate>
    inline auto to_map(TKeyPredicate&& kp)
        { return to_map(std::forward<TKeyPredicate>(kp), std::move(op_select_value_default())); }

    inline auto to_map()
        { return to_map(std::move(op_select_key_default()), std::move(op_select_value_default())); }

    template<class TPredicate>
    inline auto for_each(TPredicate&& p)
        { return __impl::for_each_builder<TPredicate>(std::forward<TPredicate>(p)); }

    template<class TKeyPredicate, class TValuePredicate>
    inline auto to_lookup(TKeyPredicate&& kp, TValuePredicate&& vp)
        { return __impl::to_lookup_builder<TKeyPredicate, TValuePredicate>(std::forward<TKeyPredicate>(kp), std::forward<TValuePredicate>(vp)); }

    template<class TKeyPredicate>
    inline auto to_lookup(TKeyPredicate&& kp)
        { return to_lookup(std::forward<TKeyPredicate>(kp), std::move(op_select_value_default())); }

    inline auto to_lookup()
        { return to_lookup(std::move(op_select_key_default()), std::move(op_select_value_default())); }
}