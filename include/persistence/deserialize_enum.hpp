#pragma once
#include "deserialize_fundamental.hpp"
#include <type_traits>

namespace persistence
{
    template<typename T>
    struct JsonDeserializer<T, typename std::enable_if_t<std::is_enum_v<T>>>
    {
        bool operator()(const rapidjson::Value& json, T& value) const
        {
            using integer_type = typename std::underlying_type<T>::type;
            JsonDeserializer<integer_type> deserializer;

            integer_type integer_value;
            if (!deserializer(json, integer_value)) {
                return false;
            }

            value = static_cast<T>(integer_value);
            return true;
        }
    };
}
