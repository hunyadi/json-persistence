#pragma once
#include "deserialize_base.hpp"
#include "detail/deserialize_aware.hpp"
#include <tuple>
#include <utility>

namespace persistence
{
    template<typename C, std::size_t S>
    struct JsonTupleDeserializer : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, C& container) const
        {
            if (!json.IsArray()) {
                return false;
            }
            if (json.Size() != S) {
                return false;
            }

            return deserialize_elements(json.Begin(), container, std::make_index_sequence<S>());
        }

    private:
        template<typename Iterator, std::size_t... Indices>
        bool deserialize_elements(Iterator it, C& container, std::index_sequence<Indices...>) const
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

    template<typename T1, typename T2>
    struct JsonDeserializer<std::pair<T1, T2>> : JsonTupleDeserializer<std::pair<T1, T2>, 2>
    {
        using JsonTupleDeserializer<std::pair<T1, T2>, 2>::JsonTupleDeserializer;
    };

    template<typename... Ts>
    struct JsonDeserializer<std::tuple<Ts...>> : JsonTupleDeserializer<std::tuple<Ts...>, sizeof...(Ts)>
    {
        using JsonTupleDeserializer<std::tuple<Ts...>, sizeof...(Ts)>::JsonTupleDeserializer;
    };
}
