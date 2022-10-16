#pragma once
#include "serialize_base.hpp"
#include "detail/serialize_aware.hpp"
#include <set>

namespace persistence
{
    template<typename T>
    struct JsonSerializer<std::set<T>> : JsonContextAwareSerializer
    {
        using JsonContextAwareSerializer::JsonContextAwareSerializer;

        bool operator()(const std::set<T>& container, rapidjson::Value& json) const
        {
            json.SetArray();
            json.Reserve(container.size(), context.global().allocator());
            std::size_t k = 0;
            for (const auto& item : container) {
                rapidjson::Value item_json;
                SerializerContext item_context(context, Segment(k));
                if (!serialize<T>(item, item_json, context)) {
                    return false;
                }
                json.PushBack(item_json, context.global().allocator());  // ownership of value is transferred
                ++k;
            }
            return true;
        }
    };
}
