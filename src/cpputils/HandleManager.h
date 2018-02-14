#pragma once

#include <limits>
#include <vector>
#include <string>
#include <cstdint>

#include "./Misc.h"
#include "./Exception.h"

namespace utl
{
    struct Handle
    {
        uint64_t value;

        inline explicit     operator bool    () const;
        inline              operator uint64_t() const;
        inline Handle&      operator =       (uint64_t);
        inline Handle&      operator =       (const Handle&);

        inline bool         operator ==      (const Handle&) const;
        inline bool         operator !=      (const Handle&) const;
        inline bool         operator <=      (const Handle&) const;
        inline bool         operator >=      (const Handle&) const;
        inline bool         operator <       (const Handle&) const;
        inline bool         operator >       (const Handle&) const;

        inline bool         operator ==      (const uint64_t&) const;
        inline bool         operator !=      (const uint64_t&) const;
        inline bool         operator <=      (const uint64_t&) const;
        inline bool         operator >=      (const uint64_t&) const;
        inline bool         operator <       (const uint64_t&) const;
        inline bool         operator >       (const uint64_t&) const;

        inline std::string  toString         () const;

        inline Handle();
        inline Handle(const uint64_t&);
        inline Handle(const Handle&);

        static inline bool          fromString(const std::string& str, Handle& handle);
        static inline const Handle& empty();
    };

    using TypeId    = uint8_t;
    using SystemId  = uint8_t;

    namespace __impl
    {
        using Index         = size_t;
        using InternIndex   = ssize_t;

        static constexpr InternIndex    INVALID_INDEX   = -1;
        static constexpr size_t         GROW_SIZE       = 100;

        enum class EntryStatus
        {
            Unknown,
            Used,
            Free,
        };

        #pragma pack(push, 1)
        struct HandleData
        {
            SystemId    systemId;   //  0..7 system id (to manage handles for multiple (dedicated) subsystems)
            TypeId      typeId;     //  8..15 type id (defined by user)
            uint16_t    counter;    // 16..31 reusage counter
            uint32_t    entryIndex; // 32..64 index in data array (unique for each system)
        };
        #pragma pack(pop)
        static_assert(sizeof(Handle) == sizeof(HandleData), "mismatching size of Handle and HandleData");
        static_assert(sizeof(Handle) == 8,                  "size of Handle is not equal 64");

        inline HandleData makeHandleData(const Handle& handle);
        inline Handle     makeHandle    (HandleData hd);

        template<class TEntry>
        struct System;

        template<class TValue>
        struct Entry
        {
        public:
            using value_type   = TValue;
            using this_type    = Entry<value_type>;
            using wrapper_type = wrapper<value_type>;

        private:
            InternIndex     _next;
            InternIndex     _prev;
            uint16_t        _counter;
            EntryStatus     _status;
            TypeId          _typeId;
            wrapper_type    _data;

        public:
            inline InternIndex  next    () const;
            inline void         next    (InternIndex value);
            inline InternIndex  prev    () const;
            inline void         prev    (InternIndex value);
            inline EntryStatus  status  () const;
            inline void         link    (InternIndex prev, InternIndex next, EntryStatus status);
            inline void         unlink  ();
            inline uint16_t     counter () const;
            inline void         assign  (value_type value, const TypeId& tId, const uint16_t cntr = 0);
            inline value_type   data    ();
            inline void         data    (value_type v);

            inline Entry();
            inline Entry(Entry&&);
            inline Entry(const Entry&);
        };

        template<class TEntry>
        struct System
        {
        public:
            using entry_type        = TEntry;
            using this_type         = System<entry_type>;
            using entry_vector_type = std::vector<entry_type>;

        private:
            InternIndex         _firstFree;
            InternIndex         _lastFree;
            InternIndex         _firstUsed;
            InternIndex         _lastUsed;
            entry_vector_type   _entries;

            inline void         pushFront       (Index index, InternIndex& first, InternIndex& last, EntryStatus status);
            inline void         pushBack        (Index index, InternIndex& first, InternIndex& last, EntryStatus status);
            inline Index        popFront        (bool canGrow, InternIndex& first, InternIndex& last, EntryStatus status);
            inline Index        popBack         (bool canGrow, InternIndex& first, InternIndex& last, EntryStatus status);
            inline void         remove          (Index index, InternIndex& first, InternIndex& last, EntryStatus status);

        public:
            inline void         grow            (size_t size = 0);
            inline void         pushFrontFree   (Index index);
            inline void         pushFrontUsed   (Index index);
            inline void         pushBackFree    (Index index);
            inline void         pushBackUsed    (Index index);
            inline Index        popFrontFree    ();
            inline Index        popFrontUsed    ();
            inline Index        popBackFree     ();
            inline Index        popBackUsed     ();
            inline void         removeFree      (Index index);
            inline void         removeUsed      (Index index);
            inline size_t       size            () const;

            inline entry_type&          operator[]  (const Index& index);
            inline const entry_type&    operator[]  (const Index& index) const;

            inline System();
            inline System(System&&);
            inline System(const System&&);
        };

        template<class TEntry>
        struct Systems
        {
        public:
            using entry_type            = TEntry;
            using this_type             = System<entry_type>;
            using system_type           = System<entry_type>;
            using system_vector_type    = std::vector<system_type>;

        private:
            system_vector_type _systems;

        public:
            inline system_type&         operator[]  (const Index& index);
            inline const system_type&   operator[]  (const Index& index) const;
            inline size_t               size        () const;
            inline void                 clear       ();
        };
    }

    inline TypeId getTypeId(const Handle& handle)
        { return __impl::makeHandleData(handle).typeId; }

    inline SystemId getSystemId(const Handle& handle)
        { return __impl::makeHandleData(handle).systemId; }

    template<class T>
    class HandleManager
    {
    public:
        using value_type            = T;
        using this_type             = HandleManager<value_type>;
        using entry_type            = __impl::Entry<value_type>;
        using systems_type          = __impl::Systems<entry_type>;

    private:
        systems_type _systems;

    public:
        /** check if an given handle is valid
         *  @param handle   handle to check
         *  @return         TRUE if handle is valid, FALSE otherwise */
        inline bool isValid(const Handle& handle) const;

        /** try to get the value for a given handle
         *  @param handle   handle to get value for
         *  @param value    parameter to store value at
         *  @return         TRUE on success, FALSE otherwise */
        inline bool tryGet(const Handle& handle, value_type& value);

        /** get the value for a given handle
         *  @param handle   handle to get value for
         *  @return         valid stored for handle */
        inline value_type get(const Handle& handle);

        /** update the value of a given handle (only valid handles are accepted)
         *  @param handle   handle to update value for
         *  @param value    new value to store
         *  @return         TRUE on success, FALSE otherwise (invalid handle) */
        inline bool update(const Handle& handle, value_type value);

        /** set the value for a given handle (this will add the item to the manager if the handle is unknown)
         *  @param handle   handle to set value for
         *  @param value    value to store
         *  @return         TRUE on success, FALSE otherwise (the storage entry is used by an other value) */
        inline bool set(const Handle& handle, value_type value);

        /** insert a new value to the manager
         *  @param tId      type id of the value
         *  @param sId      system id of the value
         *  @param value    value to add
         *  @return         Handle of the stored value */
        inline Handle insert(const TypeId& tId, const SystemId& sId, value_type value);

        /** remove the value of a given handle
         *  @param handle   handle to remove value for
         *  @return         TRUE on success, FALSE otherwise (handle is invalid) */
        inline bool remove(const Handle& handle);

        /** access values by a given handle
         *  @param handle   handle to get value for
         *  @return         reference to the value */
        inline value_type& operator[](const Handle& handle);

        /** remove all stored valus and reset the handle manager completely */
        inline void clear();
    };
}

/* HELPER ****************************************************************************************/

inline
utl::Handle::operator bool() const
    { return static_cast<bool>(value); }

inline
utl::Handle::operator uint64_t() const
    { return value; }

inline utl::Handle&
utl::Handle::operator =(uint64_t v)
{
    value = v;
    return *this;
}

inline utl::Handle&
utl::Handle::operator =(const Handle& other)
{
    value = other.value;
    return *this;
}

inline bool
utl::Handle::operator ==(const Handle& other) const
    { return value == other.value; }

inline bool
utl::Handle::operator !=(const Handle& other) const
    { return value != other.value; }

inline bool
utl::Handle::operator <=(const Handle& other) const
    { return value <= other.value; }

inline bool
utl::Handle::operator >=(const Handle& other) const
    { return value >= other.value; }

inline bool
utl::Handle::operator <(const Handle& other) const
    { return value < other.value; }

inline bool
utl::Handle::operator >(const Handle& other) const
    { return value > other.value; }

inline bool
utl::Handle::operator ==(const uint64_t& other) const
    { return value == other; }

inline bool
utl::Handle::operator !=(const uint64_t& other) const
    { return value != other; }

inline bool
utl::Handle::operator <=(const uint64_t& other) const
    { return value <= other; }

inline bool
utl::Handle::operator >=(const uint64_t& other) const
    { return value >= other; }

inline bool
utl::Handle::operator <(const uint64_t& other) const
    { return value < other; }

inline bool
utl::Handle::operator >(const uint64_t& other) const
    { return value > other; }

inline std::string
utl::Handle::toString() const
{
    char buffer[2*sizeof(*this)+3];
    memset(&buffer[0], 0, sizeof(buffer));
    const uint8_t *p = reinterpret_cast<const uint8_t*>(this);
    int x = 0;
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

inline utl::Handle::Handle() :
    value(0)
    { }

inline utl::Handle::Handle(const uint64_t& v) :
    value(v)
    { }

inline utl::Handle::Handle(const Handle& other) :
    value(other.value)
    { }

inline bool
utl::Handle::fromString(const std::string& str, Handle& handle)
{
    memset(&handle, 0, sizeof(handle));
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

inline const utl::Handle&
utl::Handle::empty()
{
    static const Handle value;
    return value;
}

inline utl::__impl::HandleData utl::__impl::makeHandleData(const Handle& handle)
{
    HandleData hd = reinterpret_cast<const HandleData&>(handle);
    hd.systemId     = ntoh(hd.systemId);
    hd.typeId       = ntoh(hd.typeId);
    hd.counter      = ntoh(hd.counter);
    hd.entryIndex   = ntoh(hd.entryIndex);
    return hd;
}

inline utl::Handle utl::__impl::makeHandle(HandleData hd)
{
    hd.systemId     = hton(hd.systemId);
    hd.typeId       = hton(hd.typeId);
    hd.counter      = hton(hd.counter);
    hd.entryIndex   = hton(hd.entryIndex);
    return reinterpret_cast<const Handle&>(hd);
}

/* ENTRY *****************************************************************************************/

template<class T>
inline utl::__impl::InternIndex
utl::__impl::Entry<T>::next() const
{
    return _next;
}

template<class T>
inline void
utl::__impl::Entry<T>::next(
    InternIndex value)
{
    _next = value;
}

template<class T>
inline utl::__impl::InternIndex
utl::__impl::Entry<T>::prev() const
{
    return _prev;
}

template<class T>
inline void
utl::__impl::Entry<T>::prev(
    InternIndex value)
{
    _prev = value;
}

template<class T>
inline utl::__impl::EntryStatus
utl::__impl::Entry<T>::status() const
{
    return _status;
}

template<class T>
inline void
utl::__impl::Entry<T>::link(InternIndex prev, InternIndex next, EntryStatus status)
{
    assert(status != EntryStatus::Unknown);
    _prev   = prev;
    _next   = next;
    _status = status;
}

template<class T>
inline void
utl::__impl::Entry<T>::unlink()
{
    _prev   = INVALID_INDEX;
    _next   = INVALID_INDEX;
    _status = EntryStatus::Unknown;
}

template<class T>
inline uint16_t
utl::__impl::Entry<T>::counter() const
    { return _counter; }

template<class T>
inline void
utl::__impl::Entry<T>::assign(
    value_type value,
    const TypeId& tId,
    const uint16_t cntr)
{
    using namespace ::utl;
    using namespace ::utl::__impl;
    assert(_status == EntryStatus::Used);
    _typeId = tId;
    _data   = value;
    if (cntr != 0)
        _counter = cntr;
    else if (++_counter == 0)
        _counter = 1;
}

template<class T>
inline typename utl::__impl::Entry<T>::value_type
utl::__impl::Entry<T>::data()
{
    return *_data;
}

template<class T>
inline void
utl::__impl::Entry<T>::data(
    value_type v)
{
    _data = v;
}

template<class T>
inline utl::__impl::Entry<T>::Entry() :
    _next   (INVALID_INDEX),
    _prev   (INVALID_INDEX),
    _counter(0),
    _status (EntryStatus::Unknown),
    _typeId (0)
    { }

template<class T>
inline utl::__impl::Entry<T>::Entry(Entry&& other) :
    _next   (std::move(other)._next),
    _prev   (std::move(other)._prev),
    _counter(std::move(other)._counter),
    _status (std::move(other)._status),
    _typeId (std::move(other)._typeId),
    _data   (std::move(other)._data)
    { }

template<class T>
inline utl::__impl::Entry<T>::Entry(const Entry& other) :
    _next   (other._next),
    _prev   (other._prev),
    _counter(other._counter),
    _status (other._status),
    _typeId (other._typeId),
    _data   (other._data)
    { }

/* SYSTEM ****************************************************************************************/

template<class T>
inline void
utl::__impl::System<T>::pushFront(
    Index index,
    InternIndex& first,
    InternIndex& last,
    EntryStatus status)
{
    assert(index < _entries.size());
    auto& entry = _entries[index];
    assert(entry.status() == EntryStatus::Unknown);
    if (first != INVALID_INDEX)
    {
        assert(first >= 0);
        assert(first < _entries.size());
        auto& firstEntry = _entries[first];
        assert(firstEntry.status() == status);
        firstEntry.prev(index);
    }
    entry.link(INVALID_INDEX, first, status);
    first = index;
    if (last == INVALID_INDEX)
        last = index;
}

template<class T>
inline void
utl::__impl::System<T>::pushBack(
    Index index,
    InternIndex& first,
    InternIndex& last,
    EntryStatus status)
{
    assert(index < _entries.size());
    auto& entry = _entries[index];
    assert(entry.status() == EntryStatus::Unknown);
    if (last != INVALID_INDEX)
    {
        assert(last >= 0);
        assert(last < _entries.size());
        auto& lastEntry = _entries[last];
        assert(lastEntry.status() == status);
        lastEntry.next(index);
    }
    entry.link(last, INVALID_INDEX, status);
    last = index;
    if (first == INVALID_INDEX)
        first = index;
}

template<class T>
inline utl::__impl::Index
utl::__impl::System<T>::popFront(
    bool canGrow,
    InternIndex& first,
    InternIndex& last,
    EntryStatus status)
{
    if (    canGrow
        && (    first >= static_cast<InternIndex>(_entries.size())
            ||  first == INVALID_INDEX))
        grow(first + GROW_SIZE);
    assert(first >= 0);
    assert(first < _entries.size());
    auto& entry = _entries[first];
    assert(entry.status() == status);
    Index ret   = first;
    if (last == first)
        last = entry.next();
    first = entry.next();
    entry.unlink();
    if (first != INVALID_INDEX)
    {
        assert(first >= 0);
        assert(first < _entries.size());
        auto& firstEntry = _entries[first];
        assert(firstEntry.status() == status);
        firstEntry.prev(INVALID_INDEX);
    }
    return ret;
}

template<class T>
inline utl::__impl::Index
utl::__impl::System<T>::popBack(
    bool canGrow,
    InternIndex& first,
    InternIndex& last,
    EntryStatus status)
{
    if (    canGrow
        &&  last >= _entries.size())
        grow(last + GROW_SIZE);
    assert(last >= 0);
    assert(last < _entries.size());
    auto& entry = _entries[last];
    assert(entry.status() == status);
    Index ret = last;
    if (first == last)
        first = entry._prev;
    last = entry._prev;
    entry.unlink();
    if (last != INVALID_INDEX)
    {
        assert(last >= 0);
        assert(last < _entries.size());
        auto& lastEntry = _entries[last];
        assert(lastEntry.status() == status);
        lastEntry.next(INVALID_INDEX);
    }
    return ret;
}

template<class T>
inline void
utl::__impl::System<T>::remove(
    Index index,
    InternIndex& first,
    InternIndex& last,
    EntryStatus status)
{
    assert(index >= 0);
    assert(index < _entries.size());
    auto& entry = _entries[index];
    assert(entry.status() == status);
    if (entry.prev() != INVALID_INDEX)
    {
        assert(entry.prev() >= 0);
        assert(entry.prev() < _entries.size());
        _entries[entry.prev()].next(entry.next());
    }
    if (entry.next() != INVALID_INDEX)
    {
        assert(entry.next() >= 0);
        assert(entry.next() < _entries.size());
        _entries[entry.next()].prev(entry.prev());
    }
    if (first == static_cast<InternIndex>(index))
        first = entry.next();
    if (last == static_cast<InternIndex>(index))
        last = entry.prev();
    entry.unlink();
}

template<class T>
inline void
utl::__impl::System<T>::grow(
    size_t size)
{
    if (size == 0)
        size = _entries.size() + GROW_SIZE;
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
utl::__impl::System<T>::pushFrontFree(
    Index index)
{
    pushFront(index, _firstFree, _lastFree, EntryStatus::Free);
}

template<class T>
inline void
utl::__impl::System<T>::pushFrontUsed(
    Index index)
{
    pushFront(index, _firstUsed, _lastUsed, EntryStatus::Used);
}

template<class T>
inline void
utl::__impl::System<T>::pushBackFree(
    Index index)
{
    pushBack(index, _firstFree, _lastFree, EntryStatus::Free);
}

template<class T>
inline void
utl::__impl::System<T>::pushBackUsed(
    Index index)
{
    pushBack(index, _firstUsed, _lastUsed, EntryStatus::Used);
}

template<class T>
inline utl::__impl::Index
utl::__impl::System<T>::popFrontFree()
{
    return popFront(true, _firstFree, _lastFree, EntryStatus::Free);
}

template<class T>
inline utl::__impl::Index
utl::__impl::System<T>::popFrontUsed()
{
    return popFront(false, _firstUsed, _lastUsed, EntryStatus::Used);
}

template<class T>
inline utl::__impl::Index
utl::__impl::System<T>::popBackFree()
{
    return popBack(true, _firstFree, _lastFree, EntryStatus::Free);
}

template<class T>
inline utl::__impl::Index
utl::__impl::System<T>::popBackUsed()
{
    return popBack(false, _firstUsed, _lastUsed, EntryStatus::Used);
}

template<class T>
inline void
utl::__impl::System<T>::removeFree(
    Index index)
{
    remove(index, _firstFree, _lastFree, EntryStatus::Free);
}

template<class T>
inline void
utl::__impl::System<T>::removeUsed(
    Index index)
{
    remove(index, _firstUsed, _lastUsed, EntryStatus::Used);
}

template<class T>
inline size_t
utl::__impl::System<T>::size() const
{
    return _entries.size();
}


template<class T>
inline typename utl::__impl::System<T>::entry_type&
utl::__impl::System<T>::operator[](
    const Index& index)
{
    if (index >= _entries.size())
        grow(index + GROW_SIZE);
    return _entries[index];
}

template<class T>
inline const typename utl::__impl::System<T>::entry_type&
utl::__impl::System<T>::operator[](
    const Index& index) const
{
    return _entries.at(index);
}

template<class T>
inline utl::__impl::System<T>::System() :
    _firstFree  (INVALID_INDEX),
    _lastFree   (INVALID_INDEX),
    _firstUsed  (INVALID_INDEX),
    _lastUsed   (INVALID_INDEX)
    { }

template<class T>
inline utl::__impl::System<T>::System(System&& other) :
    _firstFree  (std::move(other)._firstFree),
    _lastFree   (std::move(other)._lastFree),
    _firstUsed  (std::move(other)._firstUsed),
    _lastUsed   (std::move(other)._lastUsed),
    _entries    (std::move(other)._entries)
    { }

template<class T>
inline utl::__impl::System<T>::System(const System&& other) :
    _firstFree  (other._firstFree),
    _lastFree   (other._lastFree),
    _firstUsed  (other._firstUsed),
    _lastUsed   (other._lastUsed),
    _entries    (other._entries)
    { }

/* SYSTEMS ***************************************************************************************/

template<class T>
inline typename utl::__impl::Systems<T>::system_type&
utl::__impl::Systems<T>::operator[](
    const Index& index)
{
    if (index >= _systems.size())
        _systems.resize(index + 1);
    return _systems[index];
}

template<class T>
inline const typename utl::__impl::Systems<T>::system_type&
utl::__impl::Systems<T>::operator[](
    const Index& index) const
{
    return _systems[index];
}

template<class T>
inline size_t
utl::__impl::Systems<T>::size() const
{
    return _systems.size();
}

template<class T>
inline void
utl::__impl::Systems<T>::clear()
{
    _systems.clear();
}

/* HANDLEMANAGER *********************************************************************************/

template<class T>
inline bool
utl::HandleManager<T>::isValid(
    const utl::Handle& handle) const
{
    using namespace __impl;
    auto hd = makeHandleData(handle);
    if (hd.systemId >= _systems.size())
        return false;
    auto& system = _systems[hd.systemId];
    if (hd.entryIndex >= system.size())
        return false;
    auto& entry = system[hd.entryIndex];
    return entry.status() == EntryStatus::Used
        && entry.counter() == hd.counter;
}

template<class T>
inline bool
utl::HandleManager<T>::tryGet(
    const utl::Handle& handle,
    typename utl::HandleManager<T>::value_type& value)
{
    using namespace __impl;
    auto ret = isValid(handle);
    if (ret)
    {
        auto hd = makeHandleData(handle);
        value = _systems[hd.systemId][hd.entryIndex].data();
    }
    return ret;
}

template<class T>
inline typename utl::HandleManager<T>::value_type
utl::HandleManager<T>::get(
    const utl::Handle& handle)
{
    value_type ret;
    if (!tryGet(handle, ret))
        throw Exception("invalid handle");
    return ret;
}

template<class T>
inline bool
utl::HandleManager<T>::update(
    const utl::Handle& handle,
    typename utl::HandleManager<T>::value_type value)
{
    using namespace __impl;
    if (isValid(handle))
    {
        auto hd = makeHandleData(handle);
        _systems[hd.systemId][hd.entryIndex].data(value);
        return true;
    }
    return false;
}

template<class T>
inline bool
utl::HandleManager<T>::set(
    const utl::Handle& handle,
    typename utl::HandleManager<T>::value_type value)
{
    using namespace __impl;
    if (!isValid(handle))
    {
        auto  hd     = makeHandleData(handle);
        auto& system = _systems[hd.systemId];
        auto& entry  = system[hd.entryIndex];
        bool  ret    = entry.status() == EntryStatus::Free;
        if (ret)
        {
            system.removeFree  (hd.entryIndex);
            system.pushBackUsed(hd.entryIndex);
            entry.assign(value, hd.typeId, hd.counter);
        }
        return ret;
    }
    else
    {
        return update(handle, value);
    }
}

template<class T>
inline utl::Handle
utl::HandleManager<T>::insert(
    const TypeId& tId,
    const SystemId& sId,
    typename utl::HandleManager<T>::value_type value)
{
    using namespace __impl;
    auto& system = _systems[sId];
    auto  index  = system.popFrontFree();
    system.pushBackUsed(index);
    auto& entry = system[index];
    entry.assign(value, tId);
    HandleData hd;
    hd.entryIndex = static_cast<uint32_t>(index);
    hd.counter    = entry.counter();
    hd.typeId     = tId,
    hd.systemId   = sId;
    return makeHandle(hd);
}

template<class T>
inline bool
utl::HandleManager<T>::remove(
    const utl::Handle& handle)
{
    using namespace __impl;
    if (!isValid(handle))
        return false;
    auto  hd     = makeHandleData(handle);
    auto& system = _systems[hd.systemId];
    system.removeUsed(hd.entryIndex);
    system.pushFrontFree(hd.entryIndex);
    return true;
}

template<class T>
inline typename utl::HandleManager<T>::value_type&
utl::HandleManager<T>::operator[](
    const utl::Handle& handle)
{
    using namespace __impl;
    if (!isValid(handle))
        throw Exception("invalid handle");
    auto  hd     = makeHandleData(handle);
    auto& system = _systems[hd.systemId];
    auto& entry  = system[hd.entryIndex];
    return entry.data;
}

template<class T>
inline void
utl::HandleManager<T>::clear()
    { _systems.clear(); }
