#pragma once

#include <limits>
#include <vector>
#include <string>
#include <cstdint>

#include <cpputils/misc/convert.h>
#include <cpputils/misc/exception.h>
#include <cpputils/container/wrapper.h>

namespace utl
{
    struct handle
    {
        uint64_t value;

        inline explicit     operator bool    () const;
        inline              operator uint64_t() const;
        inline handle&      operator =       (uint64_t);
        inline handle&      operator =       (const handle&);

        inline bool         operator ==      (const handle&) const;
        inline bool         operator !=      (const handle&) const;
        inline bool         operator <=      (const handle&) const;
        inline bool         operator >=      (const handle&) const;
        inline bool         operator <       (const handle&) const;
        inline bool         operator >       (const handle&) const;

        inline bool         operator ==      (const uint64_t&) const;
        inline bool         operator !=      (const uint64_t&) const;
        inline bool         operator <=      (const uint64_t&) const;
        inline bool         operator >=      (const uint64_t&) const;
        inline bool         operator <       (const uint64_t&) const;
        inline bool         operator >       (const uint64_t&) const;

        inline std::string  to_string         () const;

        inline handle();
        inline handle(const uint64_t&);
        inline handle(const handle&);

        static inline bool          from_string(const std::string& str, handle& handle);
        static inline const handle& empty();
    };

    using type_id_type    = uint8_t;
    using system_id_type  = uint8_t;

    namespace __impl
    {
        using index_type = size_t;

        static constexpr index_type invalid_index(std::numeric_limits<index_type>::max());
        static constexpr size_t     grow_size    (100);

        enum class entry_status
        {
            unknown,
            used,
            free,
        };

        #pragma pack(push, 1)
        struct handle_data
        {
            system_id_type  system_id;      //  0..7 system id (to manage handles for multiple (dedicated) subsystems)
            type_id_type    type_id;        //  8..15 type id (defined by user)
            uint16_t        counter;        // 16..31 reusage counter
            uint32_t        entry_index;    // 32..64 index in data array (unique for each system)
        };
        #pragma pack(pop)
        static_assert(sizeof(handle) == sizeof(handle_data), "mismatching size of handle and handle_data");
        static_assert(sizeof(handle) == 8,                   "size of handle is not equal 64");

        inline handle_data make_handle_data(const handle& handle);
        inline handle      make_handle     (handle_data hd);

        template<class T_entry>
        struct system;

        template<class T_value>
        struct entry
        {
        public:
            using value_type   = T_value;
            using this_type    = entry<value_type>;
            using wrapper_type = wrapper<value_type>;

        private:
            index_type      _next;
            index_type      _prev;
            uint16_t        _counter;
            entry_status    _status;
            type_id_type    _type_id;
            wrapper_type    _data;

        public:
            inline index_type   next    () const;
            inline void         next    (index_type value);
            inline index_type   prev    () const;
            inline void         prev    (index_type value);
            inline entry_status status  () const;
            inline void         link    (index_type prev, index_type next, entry_status status);
            inline void         unlink  ();
            inline uint16_t     counter () const;
            inline void         assign  (value_type value, const type_id_type& tId, const uint16_t cntr = 0);
            inline value_type   data    ();
            inline void         data    (value_type v);

            inline entry();
            inline entry(entry&&);
            inline entry(const entry&);
        };

        template<class T_entry>
        struct system
        {
        public:
            using entry_type        = T_entry;
            using this_type         = system<entry_type>;
            using entry_vector_type = std::vector<entry_type>;

        private:
            index_type          _firstFree;
            index_type          _lastFree;
            index_type          _firstUsed;
            index_type          _lastUsed;
            entry_vector_type   _entries;

            inline void         pushFront       (index_type index, index_type& first, index_type& last, entry_status status);
            inline void         pushBack        (index_type index, index_type& first, index_type& last, entry_status status);
            inline index_type   popFront        (bool canGrow, index_type& first, index_type& last, entry_status status);
            inline index_type   popBack         (bool canGrow, index_type& first, index_type& last, entry_status status);
            inline void         remove          (index_type index, index_type& first, index_type& last, entry_status status);

        public:
            inline void         grow            (size_t size = 0);
            inline void         pushFrontFree   (index_type index);
            inline void         pushFrontUsed   (index_type index);
            inline void         pushBackFree    (index_type index);
            inline void         pushBackUsed    (index_type index);
            inline index_type   popFrontFree    ();
            inline index_type   popFrontUsed    ();
            inline index_type   popBackFree     ();
            inline index_type   popBackUsed     ();
            inline void         removeFree      (index_type index);
            inline void         removeUsed      (index_type index);
            inline size_t       size            () const;

            inline entry_type&          operator[]  (const index_type& index);
            inline const entry_type&    operator[]  (const index_type& index) const;

            inline system();
            inline system(system&&);
            inline system(const system&&);
        };

        template<class T_entry>
        struct systems
        {
        public:
            using entry_type            = T_entry;
            using this_type             = system<entry_type>;
            using system_type           = system<entry_type>;
            using system_vector_type    = std::vector<system_type>;

        private:
            system_vector_type _systems;

        public:
            inline system_type&         operator[]  (const index_type& index);
            inline const system_type&   operator[]  (const index_type& index) const;
            inline size_t            size        () const;
            inline void                 clear       ();
        };
    }

    inline type_id_type get_type_id(const handle& handle)
        { return __impl::make_handle_data(handle).type_id; }

    inline system_id_type get_system_id(const handle& handle)
        { return __impl::make_handle_data(handle).system_id; }

    template<class T>
    class handle_manager
    {
    public:
        using value_type            = T;
        using this_type             = handle_manager<value_type>;
        using entry_type            = __impl::entry<value_type>;
        using systems_type          = __impl::systems<entry_type>;

    private:
        systems_type _systems;

    public:
        /** check if an given handle is valid
         *  @param handle   handle to check
         *  @return         TRUE if handle is valid, FALSE otherwise */
        inline bool is_valid(const handle& handle) const;

        /** try to get the value for a given handle
         *  @param handle   handle to get value for
         *  @param value    parameter to store value at
         *  @return         TRUE on success, FALSE otherwise */
        inline bool try_get(const handle& handle, value_type& value);

        /** get the value for a given handle
         *  @param handle   handle to get value for
         *  @return         valid stored for handle */
        inline value_type get(const handle& handle);

        /** update the value of a given handle (only valid handles are accepted)
         *  @param handle   handle to update value for
         *  @param value    new value to store
         *  @return         TRUE on success, FALSE otherwise (invalid handle) */
        inline bool update(const handle& handle, value_type value);

        /** set the value for a given handle (this will add the item to the manager if the handle is unknown)
         *  @param handle   handle to set value for
         *  @param value    value to store
         *  @return         TRUE on success, FALSE otherwise (the storage entry is used by an other value) */
        inline bool set(const handle& handle, value_type value);

        /** insert a new value to the manager
         *  @param tId      type id of the value
         *  @param sId      system id of the value
         *  @param value    value to add
         *  @return         handle of the stored value */
        inline handle insert(const type_id_type& tId, const system_id_type& sId, value_type value);

        /** remove the value of a given handle
         *  @param handle   handle to remove value for
         *  @return         TRUE on success, FALSE otherwise (handle is invalid) */
        inline bool remove(const handle& handle);

        /** access values by a given handle
         *  @param handle   handle to get value for
         *  @return         reference to the value */
        inline value_type& operator[](const handle& handle);

        /** remove all stored valus and reset the handle manager completely */
        inline void clear();
    };
}

/* HELPER ****************************************************************************************/

inline
utl::handle::operator bool() const
    { return static_cast<bool>(value); }

inline
utl::handle::operator uint64_t() const
    { return value; }

inline utl::handle&
utl::handle::operator =(uint64_t v)
{
    value = v;
    return *this;
}

inline utl::handle&
utl::handle::operator =(const handle& other)
{
    value = other.value;
    return *this;
}

inline bool
utl::handle::operator ==(const handle& other) const
    { return value == other.value; }

inline bool
utl::handle::operator !=(const handle& other) const
    { return value != other.value; }

inline bool
utl::handle::operator <=(const handle& other) const
    { return value <= other.value; }

inline bool
utl::handle::operator >=(const handle& other) const
    { return value >= other.value; }

inline bool
utl::handle::operator <(const handle& other) const
    { return value < other.value; }

inline bool
utl::handle::operator >(const handle& other) const
    { return value > other.value; }

inline bool
utl::handle::operator ==(const uint64_t& other) const
    { return value == other; }

inline bool
utl::handle::operator !=(const uint64_t& other) const
    { return value != other; }

inline bool
utl::handle::operator <=(const uint64_t& other) const
    { return value <= other; }

inline bool
utl::handle::operator >=(const uint64_t& other) const
    { return value >= other; }

inline bool
utl::handle::operator <(const uint64_t& other) const
    { return value < other; }

inline bool
utl::handle::operator >(const uint64_t& other) const
    { return value > other; }

inline std::string
utl::handle::to_string() const
{
    char buffer[2*sizeof(*this)+3];
    memset(&buffer[0], 0, sizeof(buffer));
    const uint8_t *p = reinterpret_cast<const uint8_t*>(this);
    size_t x = 0;
    for (size_t i = 0; i < sizeof(*this); ++i)
    {
        if (i == 1 || i == 2 || i == 4)
        {
            buffer[2*i+x] = '-';
            ++x;
        }
        buffer[2*i+x]   = static_cast<char>((p[i] >> 4) & 0x0F);
        buffer[2*i+x+1] = static_cast<char>((p[i] >> 0) & 0x0F);
        buffer[2*i+x]   = static_cast<char>(buffer[2*i+x]   + (buffer[2*i+x]   > 9 ? 'A' - 10 : '0'));
        buffer[2*i+x+1] = static_cast<char>(buffer[2*i+x+1] + (buffer[2*i+x+1] > 9 ? 'A' - 10 : '0'));
    }
    return std::string(&buffer[0], sizeof(buffer));
}

inline utl::handle::handle() :
    value(0)
    { }

inline utl::handle::handle(const uint64_t& v) :
    value(v)
    { }

inline utl::handle::handle(const handle& other) :
    value(other.value)
    { }

inline bool
utl::handle::from_string(const std::string& str, handle& handle)
{
    handle = utl::handle();
    const char *c = str.c_str();
    const char *e = c + str.size();
    uint8_t* p = reinterpret_cast<uint8_t*>(&handle);
    size_t i = 0;
    while(c < e && i < 2*sizeof(handle))
    {
        if (*c != '-')
        {
            uint8_t v;
            if (*c >= '0' && *c <= '9')
                v = static_cast<uint8_t>(*c - '0');
            else if (*c >= 'a' && *c <= 'f')
                v = static_cast<uint8_t>(*c - 'a');
            else if (*c >= 'A' && *c <= 'F')
                v = static_cast<uint8_t>(*c - 'A');
            else
                return false;
            if (i & 1)
                v = static_cast<uint8_t>(v << 4);
            p[i >> 1] |= v;
            ++i;
        }
        ++c;
    }
    return (c == e && i == 2*sizeof(handle));
}

inline const utl::handle&
utl::handle::empty()
{
    static const handle value;
    return value;
}

inline utl::__impl::handle_data utl::__impl::make_handle_data(const handle& handle)
{
    handle_data hd = reinterpret_cast<const handle_data&>(handle);
    hd.system_id     = ntoh(hd.system_id);
    hd.type_id       = ntoh(hd.type_id);
    hd.counter      = ntoh(hd.counter);
    hd.entry_index   = ntoh(hd.entry_index);
    return hd;
}

inline utl::handle utl::__impl::make_handle(handle_data hd)
{
    hd.system_id     = hton(hd.system_id);
    hd.type_id       = hton(hd.type_id);
    hd.counter      = hton(hd.counter);
    hd.entry_index   = hton(hd.entry_index);
    return reinterpret_cast<const handle&>(hd);
}

/* ENTRY *****************************************************************************************/

template<class T>
inline utl::__impl::index_type
utl::__impl::entry<T>::next() const
{
    return _next;
}

template<class T>
inline void
utl::__impl::entry<T>::next(
    index_type value)
{
    _next = value;
}

template<class T>
inline utl::__impl::index_type
utl::__impl::entry<T>::prev() const
{
    return _prev;
}

template<class T>
inline void
utl::__impl::entry<T>::prev(
    index_type value)
{
    _prev = value;
}

template<class T>
inline utl::__impl::entry_status
utl::__impl::entry<T>::status() const
{
    return _status;
}

template<class T>
inline void
utl::__impl::entry<T>::link(index_type prev, index_type next, entry_status status)
{
    assert(status != entry_status::unknown);
    _prev   = prev;
    _next   = next;
    _status = status;
}

template<class T>
inline void
utl::__impl::entry<T>::unlink()
{
    _prev   = invalid_index;
    _next   = invalid_index;
    _status = entry_status::unknown;
}

template<class T>
inline uint16_t
utl::__impl::entry<T>::counter() const
    { return _counter; }

template<class T>
inline void
utl::__impl::entry<T>::assign(
    value_type value,
    const type_id_type& tId,
    const uint16_t cntr)
{
    using namespace ::utl;
    using namespace ::utl::__impl;
    assert(_status == entry_status::used);
    _type_id = tId;
    _data   = value;
    if (cntr != 0)
        _counter = cntr;
    else if (++_counter == 0)
        _counter = 1;
}

template<class T>
inline typename utl::__impl::entry<T>::value_type
utl::__impl::entry<T>::data()
{
    return *_data;
}

template<class T>
inline void
utl::__impl::entry<T>::data(
    value_type v)
{
    _data = v;
}

template<class T>
inline utl::__impl::entry<T>::entry() :
    _next   (invalid_index),
    _prev   (invalid_index),
    _counter(0),
    _status (entry_status::unknown),
    _type_id (0)
    { }

template<class T>
inline utl::__impl::entry<T>::entry(entry&& other) :
    _next   (std::move(other)._next),
    _prev   (std::move(other)._prev),
    _counter(std::move(other)._counter),
    _status (std::move(other)._status),
    _type_id (std::move(other)._type_id),
    _data   (std::move(other)._data)
    { }

template<class T>
inline utl::__impl::entry<T>::entry(const entry& other) :
    _next   (other._next),
    _prev   (other._prev),
    _counter(other._counter),
    _status (other._status),
    _type_id (other._type_id),
    _data   (other._data)
    { }

/* SYSTEM ****************************************************************************************/

template<class T>
inline void
utl::__impl::system<T>::pushFront(
    index_type index,
    index_type& first,
    index_type& last,
    entry_status status)
{
    assert(index < _entries.size());
    auto& entry = _entries[index];
    assert(entry.status() == entry_status::unknown);
    if (first != invalid_index)
    {
        assert(first >= 0);
        assert(first < _entries.size());
        auto& firstEntry = _entries[first];
        assert(firstEntry.status() == status);
        firstEntry.prev(index);
    }
    entry.link(invalid_index, first, status);
    first = index;
    if (last == invalid_index)
        last = index;
}

template<class T>
inline void
utl::__impl::system<T>::pushBack(
    index_type index,
    index_type& first,
    index_type& last,
    entry_status status)
{
    assert(index < _entries.size());
    auto& entry = _entries[index];
    assert(entry.status() == entry_status::unknown);
    if (last != invalid_index)
    {
        assert(last >= 0);
        assert(last < _entries.size());
        auto& lastEntry = _entries[last];
        assert(lastEntry.status() == status);
        lastEntry.next(index);
    }
    entry.link(last, invalid_index, status);
    last = index;
    if (first == invalid_index)
        first = index;
}

template<class T>
inline utl::__impl::index_type
utl::__impl::system<T>::popFront(
    bool canGrow,
    index_type& first,
    index_type& last,
    entry_status status)
{
    if (    canGrow
        && (    first >= _entries.size()
            ||  first == invalid_index))
        grow((first != invalid_index ? first : 0) + grow_size);
    assert(first >= 0);
    assert(first < _entries.size());
    auto& entry = _entries[first];
    assert(entry.status() == status);
    index_type ret = first;
    if (last == first)
        last = entry.next();
    first = entry.next();
    entry.unlink();
    if (first != invalid_index)
    {
        assert(first >= 0);
        assert(first < _entries.size());
        auto& firstEntry = _entries[first];
        assert(firstEntry.status() == status);
        firstEntry.prev(invalid_index);
    }
    return ret;
}

template<class T>
inline utl::__impl::index_type
utl::__impl::system<T>::popBack(
    bool canGrow,
    index_type& first,
    index_type& last,
    entry_status status)
{
    if (    canGrow
        && (    last >= _entries.size()
            ||  last == invalid_index))
        grow((last != invalid_index ? last : 0) + grow_size);
    assert(last >= 0);
    assert(last < _entries.size());
    auto& entry = _entries[last];
    assert(entry.status() == status);
    index_type ret = last;
    if (first == last)
        first = entry._prev;
    last = entry._prev;
    entry.unlink();
    if (last != invalid_index)
    {
        assert(last >= 0);
        assert(last < _entries.size());
        auto& lastEntry = _entries[last];
        assert(lastEntry.status() == status);
        lastEntry.next(invalid_index);
    }
    return ret;
}

template<class T>
inline void
utl::__impl::system<T>::remove(
    index_type index,
    index_type& first,
    index_type& last,
    entry_status status)
{
    assert(index >= 0);
    assert(index < _entries.size());
    auto& entry = _entries[index];
    assert(entry.status() == status);
    if (entry.prev() != invalid_index)
    {
        assert(entry.prev() >= 0);
        assert(entry.prev() < _entries.size());
        _entries[entry.prev()].next(entry.next());
    }
    if (entry.next() != invalid_index)
    {
        assert(entry.next() >= 0);
        assert(entry.next() < _entries.size());
        _entries[entry.next()].prev(entry.prev());
    }
    if (first == index)
        first = entry.next();
    if (last == index)
        last = entry.prev();
    entry.unlink();
}

template<class T>
inline void
utl::__impl::system<T>::grow(
    size_t size)
{
    if (size == 0)
        size = _entries.size() + grow_size;
    if (size < _entries.size())
        return;
    size_t idx = _entries.size();
    _entries.resize(size);
    while (idx < _entries.size())
    {
        pushBackFree(idx);
        ++idx;
    }
}

template<class T>
inline void
utl::__impl::system<T>::pushFrontFree(
    index_type index)
{
    pushFront(index, _firstFree, _lastFree, entry_status::free);
}

template<class T>
inline void
utl::__impl::system<T>::pushFrontUsed(
    index_type index)
{
    pushFront(index, _firstUsed, _lastUsed, entry_status::used);
}

template<class T>
inline void
utl::__impl::system<T>::pushBackFree(
    index_type index)
{
    pushBack(index, _firstFree, _lastFree, entry_status::free);
}

template<class T>
inline void
utl::__impl::system<T>::pushBackUsed(
    index_type index)
{
    pushBack(index, _firstUsed, _lastUsed, entry_status::used);
}

template<class T>
inline utl::__impl::index_type
utl::__impl::system<T>::popFrontFree()
{
    return popFront(true, _firstFree, _lastFree, entry_status::free);
}

template<class T>
inline utl::__impl::index_type
utl::__impl::system<T>::popFrontUsed()
{
    return popFront(false, _firstUsed, _lastUsed, entry_status::used);
}

template<class T>
inline utl::__impl::index_type
utl::__impl::system<T>::popBackFree()
{
    return popBack(true, _firstFree, _lastFree, entry_status::free);
}

template<class T>
inline utl::__impl::index_type
utl::__impl::system<T>::popBackUsed()
{
    return popBack(false, _firstUsed, _lastUsed, entry_status::used);
}

template<class T>
inline void
utl::__impl::system<T>::removeFree(
    index_type index)
{
    remove(index, _firstFree, _lastFree, entry_status::free);
}

template<class T>
inline void
utl::__impl::system<T>::removeUsed(
    index_type index)
{
    remove(index, _firstUsed, _lastUsed, entry_status::used);
}

template<class T>
inline size_t
utl::__impl::system<T>::size() const
{
    return _entries.size();
}


template<class T>
inline typename utl::__impl::system<T>::entry_type&
utl::__impl::system<T>::operator[](
    const index_type& index)
{
    if (index >= _entries.size())
        grow(index + grow_size);
    return _entries[index];
}

template<class T>
inline const typename utl::__impl::system<T>::entry_type&
utl::__impl::system<T>::operator[](
    const index_type& index) const
{
    return _entries.at(index);
}

template<class T>
inline utl::__impl::system<T>::system() :
    _firstFree  (invalid_index),
    _lastFree   (invalid_index),
    _firstUsed  (invalid_index),
    _lastUsed   (invalid_index)
    { }

template<class T>
inline utl::__impl::system<T>::system(system&& other) :
    _firstFree  (std::move(other)._firstFree),
    _lastFree   (std::move(other)._lastFree),
    _firstUsed  (std::move(other)._firstUsed),
    _lastUsed   (std::move(other)._lastUsed),
    _entries    (std::move(other)._entries)
    { }

template<class T>
inline utl::__impl::system<T>::system(const system&& other) :
    _firstFree  (other._firstFree),
    _lastFree   (other._lastFree),
    _firstUsed  (other._firstUsed),
    _lastUsed   (other._lastUsed),
    _entries    (other._entries)
    { }

/* SYSTEMS ***************************************************************************************/

template<class T>
inline typename utl::__impl::systems<T>::system_type&
utl::__impl::systems<T>::operator[](
    const index_type& index)
{
    if (index >= _systems.size())
        _systems.resize(index + 1);
    return _systems[index];
}

template<class T>
inline const typename utl::__impl::systems<T>::system_type&
utl::__impl::systems<T>::operator[](
    const index_type& index) const
{
    return _systems[index];
}

template<class T>
inline size_t
utl::__impl::systems<T>::size() const
{
    return _systems.size();
}

template<class T>
inline void
utl::__impl::systems<T>::clear()
{
    _systems.clear();
}

/* handleMANAGER *********************************************************************************/

template<class T>
inline bool
utl::handle_manager<T>::is_valid(
    const utl::handle& handle) const
{
    using namespace __impl;
    auto hd = make_handle_data(handle);
    if (hd.system_id >= _systems.size())
        return false;
    auto& system = _systems[hd.system_id];
    if (hd.entry_index >= system.size())
        return false;
    auto& entry = system[hd.entry_index];
    return entry.status() == entry_status::used
        && entry.counter() == hd.counter;
}

template<class T>
inline bool
utl::handle_manager<T>::try_get(
    const utl::handle& handle,
    typename utl::handle_manager<T>::value_type& value)
{
    using namespace __impl;
    auto ret = is_valid(handle);
    if (ret)
    {
        auto hd = make_handle_data(handle);
        value = _systems[hd.system_id][hd.entry_index].data();
    }
    return ret;
}

template<class T>
inline typename utl::handle_manager<T>::value_type
utl::handle_manager<T>::get(
    const utl::handle& handle)
{
    value_type ret;
    if (!try_get(handle, ret))
        throw exception("invalid handle");
    return ret;
}

template<class T>
inline bool
utl::handle_manager<T>::update(
    const utl::handle& handle,
    typename utl::handle_manager<T>::value_type value)
{
    using namespace __impl;
    if (is_valid(handle))
    {
        auto hd = make_handle_data(handle);
        _systems[hd.system_id][hd.entry_index].data(value);
        return true;
    }
    return false;
}

template<class T>
inline bool
utl::handle_manager<T>::set(
    const utl::handle& handle,
    typename utl::handle_manager<T>::value_type value)
{
    using namespace __impl;
    if (!is_valid(handle))
    {
        auto  hd     = make_handle_data(handle);
        auto& system = _systems[hd.system_id];
        auto& entry  = system[hd.entry_index];
        bool  ret    = entry.status() == entry_status::free;
        if (ret)
        {
            system.removeFree  (hd.entry_index);
            system.pushBackUsed(hd.entry_index);
            entry.assign(value, hd.type_id, hd.counter);
        }
        return ret;
    }
    else
    {
        return update(handle, value);
    }
}

template<class T>
inline utl::handle
utl::handle_manager<T>::insert(
    const type_id_type& tId,
    const system_id_type& sId,
    typename utl::handle_manager<T>::value_type value)
{
    using namespace __impl;
    auto& system = _systems[sId];
    auto  index  = system.popFrontFree();
    system.pushBackUsed(index);
    auto& entry = system[index];
    entry.assign(value, tId);
    handle_data hd;
    hd.entry_index = static_cast<uint32_t>(index);
    hd.counter    = entry.counter();
    hd.type_id     = tId,
    hd.system_id   = sId;
    return make_handle(hd);
}

template<class T>
inline bool
utl::handle_manager<T>::remove(
    const utl::handle& handle)
{
    using namespace __impl;
    if (!is_valid(handle))
        return false;
    auto  hd     = make_handle_data(handle);
    auto& system = _systems[hd.system_id];
    system.removeUsed(hd.entry_index);
    system.pushFrontFree(hd.entry_index);
    return true;
}

template<class T>
inline typename utl::handle_manager<T>::value_type&
utl::handle_manager<T>::operator[](
    const utl::handle& handle)
{
    using namespace __impl;
    if (!is_valid(handle))
        throw exception("invalid handle");
    auto  hd     = make_handle_data(handle);
    auto& system = _systems[hd.system_id];
    auto& entry  = system[hd.entry_index];
    return entry.data;
}

template<class T>
inline void
utl::handle_manager<T>::clear()
    { _systems.clear(); }
