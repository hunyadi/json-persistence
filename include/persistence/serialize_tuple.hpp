#pragma once
#include "serialize_allocate.hpp"
#include "serialize_base.hpp"
#include <tuple>
#include <utility>

namespace persistence
{
    template<typename T1, typename T2>
    struct JsonSerializer<std::pair<T1, T2>> : JsonAllocatingSerializer
    {
        using JsonAllocatingSerializer::JsonAllocatingSerializer;

        bool operator()(const std::pair<T1, T2>& container, rapidjson::Value& json) const
        {
            json.SetArray();
            json.Reserve(2, allocator);
            rapidjson::Value json_first, json_second;
            if (!serialize(container.first, json_first, allocator)) {
                return false;
            }
            if (!serialize(container.second, json_second, allocator)) {
                return false;
            }
            json.PushBack(json_first, allocator);
            json.PushBack(json_second, allocator);
            return true;
        }
    };

    template<typename... Ts>
    struct JsonSerializer<std::tuple<Ts...>> : JsonAllocatingSerializer
    {
        using JsonAllocatingSerializer::JsonAllocatingSerializer;

        bool operator()(const std::tuple<Ts...>& container, rapidjson::Value& json) const
        {
            json.SetArray();
            json.Reserve(sizeof...(Ts), allocator);
            return serialize_elements(container, json, std::make_index_sequence<sizeof...(Ts)>());
        }

    private:
        template<std::size_t... I>
        bool serialize_elements(const std::tuple<Ts...>& container, rapidjson::Value& json, std::index_sequence<I...>) const
        {
            return (serialize_element(
                std::get<I>(container),
                json
            ) && ...);
        }

        template<typename T>
        bool serialize_element(const T& item, rapidjson::Value& json) const
        {
            rapidjson::Value json_item;
            if (!serialize(item, json_item, allocator)) {
                return false;
            }

            json.PushBack(json_item, allocator);  // ownership of value is transferred
            return true;
        }
    };
}
