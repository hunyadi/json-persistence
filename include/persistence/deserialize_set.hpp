#pragma once
#include "deserialize_base.hpp"
#include "detail/engine.hpp"
#include <set>

namespace persistence
{
    template<typename T>
    struct JsonDeserializer<std::set<T>>
    {
        bool operator()(const rapidjson::Value& json, std::set<T>& container) const
        {
            if (!json.IsArray()) {
                return false;
            }

            container.clear();
            for (auto&& it = json.Begin(); it != json.End(); ++it) {
                T item;
                if (!deserialize(*it, item)) {
                    return false;
                }
                container.insert(std::move(item));
            }
            return true;
        }
    };
}
