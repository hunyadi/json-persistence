#pragma once
#include <map>

namespace persistence
{
    template<typename K, typename V>
    struct ReferenceTable
    {
        V get(K key) const
        {
            auto&& it = references.find(key);
            if (it != references.end()) {
                return it->second;
            } else {
                return V();
            }
        }

        void put(K key, V value)
        {
            references.insert(std::make_pair(key, value));
        }

    private:
        std::map<K, V> references;
    };
}
