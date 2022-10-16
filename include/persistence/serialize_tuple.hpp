#pragma once
#include "serialize_base.hpp"
#include "detail/serialize_aware.hpp"
#include <tuple>
#include <utility>

namespace persistence
{
    template<typename T1, typename T2>
    struct JsonSerializer<std::pair<T1, T2>> : JsonContextAwareSerializer
    {
        using JsonContextAwareSerializer::JsonContextAwareSerializer;

        bool operator()(const std::pair<T1, T2>& container, rapidjson::Value& json) const
        {
            json.SetArray();
            json.Reserve(2, context.global().allocator());
            rapidjson::Value json_first;
            SerializerContext context_first(context, Segment(0));
            if (!serialize(container.first, json_first, context_first)) {
                return false;
            }
            rapidjson::Value json_second;
            SerializerContext context_second(context, Segment(1));
            if (!serialize(container.second, json_second, context_second)) {
                return false;
            }
            json.PushBack(json_first, context.global().allocator());
            json.PushBack(json_second, context.global().allocator());
            return true;
        }
    };

    template<typename... Ts>
    struct JsonSerializer<std::tuple<Ts...>> : JsonContextAwareSerializer
    {
        using JsonContextAwareSerializer::JsonContextAwareSerializer;

        bool operator()(const std::tuple<Ts...>& container, rapidjson::Value& json) const
        {
            json.SetArray();
            json.Reserve(sizeof...(Ts), context.global().allocator());
            return serialize_elements(container, json, std::make_index_sequence<sizeof...(Ts)>());
        }

    private:
        template<std::size_t... I>
        bool serialize_elements(const std::tuple<Ts...>& container, rapidjson::Value& json, std::index_sequence<I...>) const
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
            if (!serialize(item, item_json, item_context)) {
                return false;
            }

            json.PushBack(item_json, context.global().allocator());  // ownership of value is transferred
            return true;
        }
    };
}
