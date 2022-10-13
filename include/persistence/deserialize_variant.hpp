#pragma once
#include "deserialize_base.hpp"
#include "detail/deserialize_aware.hpp"
#include <variant>

namespace persistence
{
    template<typename... Ts>
    struct JsonDeserializer<std::variant<Ts...>> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, std::variant<Ts...>& value) const
        {
            return (deserialize_variant<Ts>(json, value) || ...);
        }

    private:
        template<typename T>
        bool deserialize_variant(const rapidjson::Value& json, std::variant<Ts...>& value) const
        {
            T var;
            if (deserialize(json, var, context)) {
                value = std::move(var);
                return true;
            } else {
                return false;
            }
        }
    };
}
