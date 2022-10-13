#pragma once
#include "deserialize_base.hpp"
#include <vector>

namespace persistence
{
    template<typename T>
    struct JsonDeserializer<std::vector<T>>
    {
        bool operator()(const rapidjson::Value& json, std::vector<T>& container) const
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
                container.push_back(std::move(item));
            }
            return true;
        }
    };
}
