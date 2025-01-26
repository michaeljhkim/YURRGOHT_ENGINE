#pragma once


namespace meta_hpp::detail
{
    template < typename Key, typename Value, typename Compare, typename Allocator >
    void insert( //
        std::multimap<Key, Value, Compare, Allocator>& multimap,
        std::multimap<Key, Value, Compare, Allocator>& value
    ) {
        multimap.swap(value);
        multimap.merge(value);
    }

    template < typename Key, typename Value, typename Compare, typename Allocator >
    void insert( //
        std::multimap<Key, Value, Compare, Allocator>& multimap,
        std::multimap<Key, Value, Compare, Allocator>&& value
    ) {
        multimap.swap(value);
        multimap.merge(std::move(value));
    }
}
