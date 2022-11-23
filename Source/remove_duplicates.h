#pragma once
#include "search_server.h"
#include <map>
#include <set>


void RemoveDuplicates(SearchServer& search_server);

template <typename Key, typename Data>
std::set<Key> ExtractOfKeys(std::map<Key, Data> this_map) {
    std::set<Key> return_map;
    for (const auto [key, data] : this_map) {
        return_map.insert(key);
    }

    return return_map;
}