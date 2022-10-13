#pragma once
#include "deserialize_base.hpp"
#include "detail/deserialize_aware.hpp"
#include <array>

namespace persistence
{
    template<typename T, std::size_t N>
    struct JsonDeserializer<std::array<T, N>> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, std::array<T, N>& container) const
        {
            if (!json.IsArray()) {
                return false;
            }

            std::array<T, N> output;
            std::size_t idx = 0;
            for (auto&& it = json.Begin(); it != json.End(); ++it) {
                if (idx >= N) {
                    return false;  // more elements in JSON than size of std::array
                }

                T item;
                DeserializerContext item_context(context, Segment(idx));
                if (!deserialize(*it, item, item_context)) {
                    return false;
                }
                output[idx++] = item;
            }
            if (idx != N) {
                return false;  // fewer elements in JSON than size of std::array
            }
            container = std::move(output);
            return true;
        }
    };
}
