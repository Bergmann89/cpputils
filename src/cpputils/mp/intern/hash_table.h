#pragma once

#include <cpputils/mp/operations/hash.h>
#include <cpputils/mp/operations/compare/equal.h>

namespace utl {
namespace mp {
namespace intern {

    namespace __impl
    {
        template<typename H, size_t... I>
        struct bucket { };

        template<typename... Buckets>
        struct hash_table
            : Buckets...
            { };

        /* find indices */
        template<typename Hash, size_t... I>
        std::index_sequence<I...> find_indices_impl(const bucket<Hash, I...>&);

        template<typename Hash>
        std::index_sequence<> find_indices_impl(...);

        /* find index */
        template<template<size_t> class KeyAtIndex, typename Key>
        struct find_pred
        {
            template<typename Index>
            auto operator()(const Index&) const ->
                decltype(equal(std::declval<KeyAtIndex<Index::value>>(), std::declval<Key>()));
        };

        template<typename Indices, typename Key, template<size_t> class KeyAtIndex>
        struct find_index_impl
            { using type = decltype(find_if(Indices { }, find_pred<KeyAtIndex, Key> { })); };
    }

    template<typename Map, typename Key>
    struct find_indices
    {
        using hash = typename decltype(hash(std::declval<Key>()))::type;
        using type = decltype(__impl::find_indices_impl<hash>(std::declval<Map>()));
    };

    template<template<size_t> class KeyAtIndex, size_t N, typename Indices = std::make_index_sequence<N>>
    struct make_hash_table;

    template<template<size_t> class KeyAtIndex, size_t N, size_t... I>
    struct make_hash_table<KeyAtIndex, N, std::index_sequence<I...>>
    {
        using type = __impl::hash_table<
            __impl::bucket<typename decltype(hash(std::declval<KeyAtIndex<I>>()))::type, I>...
        >;
    };

}
}
}