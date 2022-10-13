#pragma once
#include "deserialize_base.hpp"
#include "detail/deserialize_aware.hpp"
#include <set>

namespace persistence
{
    template<typename T>
    struct JsonDeserializer<std::set<T>> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, std::set<T>& container) const
        {
            if (!json.IsArray()) {
                return false;
            }

            container.clear();
            std::size_t k = 0;
            for (auto&& it = json.Begin(); it != json.End(); ++it) {
                T item;
                DeserializerContext item_context(context, Segment(k));
                if (!deserialize(*it, item, item_context)) {
                    return false;
                }
                container.insert(std::move(item));
                ++k;
            }
            return true;
        }
    };
}
