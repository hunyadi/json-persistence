#pragma once
#include "deserialize_base.hpp"
#include "deserialize_check.hpp"
#include "detail/deserialize_aware.hpp"
#include <array>

namespace persistence
{
    template<bool Exception, typename T, std::size_t N>
    struct JsonDeserializer<Exception, std::array<T, N>> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, std::array<T, N>& container) const
        {
            if (!detail::check_array<Exception>(json, context)) {
                return false;
            }
            if (!detail::check_size<Exception, N>(json, context)) {
                return false;
            }

            std::array<T, N> output;
            std::size_t idx = 0;
            for (auto&& it = json.Begin(); it != json.End(); ++it) {
                T item;
                DeserializerContext item_context(context, Segment(idx));
                if (!deserialize<Exception>(*it, item, item_context)) {
                    return false;
                }
                output[idx++] = item;
            }
            container = std::move(output);
            return true;
        }
    };
}
