#pragma once
#include "deserialize_base.hpp"
#include "detail/deserialize_aware.hpp"
#include <vector>

namespace persistence
{
    template<typename T>
    struct JsonDeserializer<std::vector<T>> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, std::vector<T>& container) const
        {
            if (!json.IsArray()) {
                return false;
            }

            container.clear();
            std::size_t idx = 0;
            for (auto&& it = json.Begin(); it != json.End(); ++it) {
                T item;
                DeserializerContext item_context(context, Segment(idx));
                if (!deserialize(*it, item, item_context)) {
                    return false;
                }
                container.push_back(std::move(item));
                ++idx;
            }
            return true;
        }
    };
}
