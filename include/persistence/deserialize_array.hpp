#pragma once
#include "deserialize_base.hpp"
#include "deserialize_check.hpp"
#include "detail/deserialize_aware.hpp"
#include "detail/unlikely.hpp"
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
            T* out = output.data();
            std::size_t index = 0;
            for (auto&& it = json.Begin(); it != json.End(); ++it, ++out, ++index) {
                T item;
                DeserializerContext item_context(context, Segment(index));
                PERSISTENCE_IF_UNLIKELY(!deserialize<Exception>(*it, item, item_context)) {
                    return false;
                }
                *out = item;
            }
            container = std::move(output);
            return true;
        }
    };
}
