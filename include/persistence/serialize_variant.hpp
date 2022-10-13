#pragma once
#include "serialize_base.hpp"
#include "detail/serialize_aware.hpp"
#include <variant>

namespace persistence
{
    template<typename... Ts>
    struct JsonSerializer<std::variant<Ts...>> : JsonContextAwareSerializer
    {
        using JsonContextAwareSerializer::JsonContextAwareSerializer;

        bool operator()(const std::variant<Ts...>& value, rapidjson::Value& json) const
        {
            return std::visit(
                [&](auto&& arg) -> bool {
                    return serialize(arg, json, context);
                },
                value
            );
        }
    };
}
