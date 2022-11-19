#pragma once
#include "serialize_base.hpp"
#include "detail/serialize_aware.hpp"
#include "detail/unlikely.hpp"
#include <tuple>
#include <utility>

namespace persistence
{
    template<typename C, std::size_t S>
    struct JsonTupleSerializer : JsonContextAwareSerializer
    {
        using JsonContextAwareSerializer::JsonContextAwareSerializer;

        bool operator()(const C& container, rapidjson::Value& json) const
        {
            json.SetArray();
            json.Reserve(S, context.global().allocator());
            return serialize_elements(container, json, std::make_index_sequence<S>());
        }

    private:
        template<std::size_t... I>
        bool serialize_elements(const C& container, rapidjson::Value& json, std::index_sequence<I...>) const
        {
            return (serialize_element(
                I,
                std::get<I>(container),
                json
            ) && ...);
        }

        template<typename T>
        bool serialize_element(std::size_t index, const T& item, rapidjson::Value& json) const
        {
            rapidjson::Value item_json;
            SerializerContext item_context(context, Segment(index));
            PERSISTENCE_IF_UNLIKELY(!serialize(item, item_json, item_context)) {
                return false;
            }

            json.PushBack(item_json, context.global().allocator());
            return true;
        }
    };

    template<typename T1, typename T2>
    struct JsonSerializer<std::pair<T1, T2>> : JsonTupleSerializer<std::pair<T1, T2>, 2>
    {
        using JsonTupleSerializer<std::pair<T1, T2>, 2>::JsonTupleSerializer;
    };

    template<typename... Ts>
    struct JsonSerializer<std::tuple<Ts...>> : JsonTupleSerializer<std::tuple<Ts...>, sizeof...(Ts)>
    {
        using JsonTupleSerializer<std::tuple<Ts...>, sizeof...(Ts)>::JsonTupleSerializer;
    };
}
