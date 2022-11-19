#pragma once
#include "serialize_base.hpp"
#include "detail/serialize_aware.hpp"
#include "detail/unlikely.hpp"
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
            json.Reserve(static_cast<rapidjson::SizeType>(container.size()), context.global().allocator());
            std::size_t idx = 0;
            for (const auto& item : container) {
                rapidjson::Value item_json;
                SerializerContext item_context(context, Segment(idx));
                PERSISTENCE_IF_UNLIKELY(!serialize<T>(item, item_json, item_context)) {
                    return false;
                }
                json.PushBack(item_json, context.global().allocator());  // ownership of value is transferred
                ++idx;
            }
            return true;
        }
    };
}
