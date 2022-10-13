#pragma once
#include "serialize_base.hpp"
#include "detail/serialize_aware.hpp"
#include <array>

namespace persistence
{
    template<typename T, std::size_t N>
    struct JsonSerializer<std::array<T, N>> : JsonContextAwareSerializer
    {
        using JsonContextAwareSerializer::JsonContextAwareSerializer;

        bool operator()(const std::array<T, N>& container, rapidjson::Value& json) const
        {
            json.SetArray();
            json.Reserve(container.size(), context.allocator());
            std::size_t idx = 0;
            for (const auto& item : container) {
                rapidjson::Value item_json;
                SerializerContext item_context(context, Segment(idx));
                if (!serialize<T>(item, item_json, item_context)) {
                    return false;
                }
                json.PushBack(item_json, context.allocator());  // ownership of value is transferred
                ++idx;
            }
            return true;
        }
    };
}
