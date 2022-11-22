#pragma once
#include <map>
#include <optional>

namespace persistence
{
    template<typename K, typename V>
    struct ReferenceTable
    {
        std::optional<V> get(const K& key) const
        {
            auto&& it = references.find(key);
            if (it != references.end()) {
                return it->second;
            } else {
                return std::nullopt;
            }
        }

        void put(const K& key, const V& value)
        {
            references.insert(std::make_pair(key, value));
        }

        void put(K&& key, V&& value)
        {
            references.insert(std::make_pair(std::move(key), std::move(value)));
        }

    private:
        std::map<K, V> references;
    };
}
