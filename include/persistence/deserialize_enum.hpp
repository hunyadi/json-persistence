#pragma once
#include "enum.hpp"
#include "deserialize_fundamental.hpp"

namespace persistence
{
    template<typename T>
    struct JsonDeserializer<T, typename std::enable_if_t<std::is_enum_v<T>>>
    {
        bool operator()(const rapidjson::Value& json, T& value) const
        {
            if constexpr (detect<T, enum_from_string_function>::value) {
                static_assert(
                    std::is_same_v<enum_from_string_function<T>, bool>,
                    "expected function with signature: static bool from_string(const std::string_view&, Enum&)"
                );

                if (!json.IsString()) {
                    return false;
                }

                std::string_view view(json.GetString(), json.GetStringLength());
                if (!enum_traits<T>::from_string(view, value)) {
                    return false;
                }
                return true;
            } else {
                using integer_type = typename std::underlying_type<T>::type;
                JsonDeserializer<integer_type> deserializer;

                integer_type integer_value;
                if (!deserializer(json, integer_value)) {
                    return false;
                }

                value = static_cast<T>(integer_value);
                return true;
            }
        }
    };
}
