#pragma once
#include "deserialize_base.hpp"
#include "deserialize_check.hpp"
#include "detail/deserialize_aware.hpp"
#include <tuple>
#include <utility>

namespace persistence
{
    template<bool Exception, typename C, std::size_t S>
    struct JsonTupleDeserializer : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, C& container) const
        {
            if (!detail::check_array<Exception>(json, context)) {
                return false;
            }
            if (!detail::check_size<Exception, S>(json, context)) {
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
            return deserialize<Exception>(json, item, item_context);
        }
    };

    template<bool Exception, typename T1, typename T2>
    struct JsonDeserializer<Exception, std::pair<T1, T2>> : JsonTupleDeserializer<Exception, std::pair<T1, T2>, 2>
    {
        using JsonTupleDeserializer<Exception, std::pair<T1, T2>, 2>::JsonTupleDeserializer;
    };

    template<bool Exception, typename... Ts>
    struct JsonDeserializer<Exception, std::tuple<Ts...>> : JsonTupleDeserializer<Exception, std::tuple<Ts...>, sizeof...(Ts)>
    {
        using JsonTupleDeserializer<Exception, std::tuple<Ts...>, sizeof...(Ts)>::JsonTupleDeserializer;
    };
}
