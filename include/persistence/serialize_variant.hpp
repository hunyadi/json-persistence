#pragma once
#include "serialize_allocate.hpp"
#include "serialize_base.hpp"
#include <variant>

namespace persistence
{
    template<typename... Ts>
    struct JsonSerializer<std::variant<Ts...>> : JsonAllocatingSerializer
    {
        using JsonAllocatingSerializer::JsonAllocatingSerializer;

        bool operator()(const std::variant<Ts...>& value, rapidjson::Value& json) const
        {
            return std::visit(
                [&](auto&& arg) -> bool {
                    return serialize(arg, json, allocator);
                },
                value
            );
        }
    };
}
