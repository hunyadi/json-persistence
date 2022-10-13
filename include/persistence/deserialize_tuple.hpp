#pragma once
#include "deserialize_base.hpp"
#include "detail/deserialize_aware.hpp"
#include <tuple>
#include <utility>

namespace persistence
{
    template<typename T1, typename T2>
    struct JsonDeserializer<std::pair<T1, T2>> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, std::pair<T1, T2>& container) const
        {
            if (!json.IsArray()) {
                return false;
            }
            if (json.Size() != 2) {
                return false;
            }

            return deserialize_item<0>(json, container)
                && deserialize_item<1>(json, container);
        }

    private:
        template<std::size_t Index>
        bool deserialize_item(const rapidjson::Value& json, std::pair<T1, T2>& container) const
        {
            auto&& json_array = json.GetArray();
            DeserializerContext item_context(context, Segment(Index));
            return deserialize(json_array[Index], std::get<Index>(container), item_context);
        }
    };

    template<typename... Ts>
    struct JsonDeserializer<std::tuple<Ts...>> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, std::tuple<Ts...>& container) const
        {
            if (!json.IsArray()) {
                return false;
            }
            if (sizeof...(Ts) != json.Size()) {
                return false;
            }

            return deserialize_elements(json.Begin(), container, std::make_index_sequence<sizeof...(Ts)>());
        }

    private:
        template<typename Iterator, std::size_t... Indices>
        bool deserialize_elements(Iterator it, std::tuple<Ts...>& container, std::index_sequence<Indices...>) const
        {
            return (deserialize_item<Indices>(
                *it++,
                std::get<Indices>(container)
            ) && ...);
        }

        template<std::size_t Index, typename T>
        bool deserialize_item(const rapidjson::Value& json, T& item) const
        {
            DeserializerContext item_context(context, Segment(Index));
            return deserialize(json, item, item_context);
        }
    };
}
