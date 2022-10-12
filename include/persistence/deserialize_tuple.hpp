#pragma once
#include "deserialize_base.hpp"
#include "detail/engine.hpp"
#include <tuple>
#include <utility>

namespace persistence
{
    template<typename T1, typename T2>
    struct JsonDeserializer<std::pair<T1, T2>>
    {
        bool operator()(const rapidjson::Value& json, std::pair<T1, T2>& container) const
        {
            if (!json.IsArray()) {
                return false;
            }
            if (json.Size() != 2) {
                return false;
            }

            auto&& json_array = json.GetArray();
            return deserialize(json_array[0], container.first) && deserialize(json_array[1], container.second);
        }
    };

    template<typename... Ts>
    struct JsonDeserializer<std::tuple<Ts...>>
    {
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
            return (deserialize(
                *it++,
                std::get<Indices>(container)
            ) && ...);
        }
    };
}
