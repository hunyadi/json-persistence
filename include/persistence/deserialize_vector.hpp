#pragma once
#include "deserialize_base.hpp"
#include "deserialize_check.hpp"
#include "detail/deserialize_aware.hpp"
#include "detail/unlikely.hpp"
#include <vector>

namespace persistence
{
    template<bool Exception, typename T>
    struct JsonDeserializer<Exception, std::vector<T>> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, std::vector<T>& container) const
        {
            if (!detail::check_array<Exception>(json, context)) {
                return false;
            }

            container.clear();
            std::size_t idx = 0;
            for (auto&& it = json.Begin(); it != json.End(); ++it) {
                T item;
                DeserializerContext item_context(context, Segment(idx));
                PERSISTENCE_IF_UNLIKELY(!deserialize<Exception>(*it, item, item_context)) {
                    return false;
                }
                container.push_back(std::move(item));
                ++idx;
            }
            return true;
        }
    };
}
