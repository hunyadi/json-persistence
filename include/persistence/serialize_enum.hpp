#pragma once
#include "enum.hpp"
#include "serialize_fundamental.hpp"
#include "serialize_string.hpp"

namespace persistence
{
    template<typename T>
    struct JsonSerializer<T, std::enable_if_t<std::is_enum_v<T>>>
    {
        bool operator()(T value, rapidjson::Value& json) const
        {
            if constexpr (detect<T, enum_to_string_function>::value) {
                static_assert(
                    std::is_same_v<enum_to_string_function<T>, std::string_view>,
                    "expected function with signature: static std::string_view to_string(Enum)"
                );
                JsonSerializer<std::string_view> string_serializer;
                return string_serializer(enum_traits<T>::to_string(value), json);
            } else {
                using integer_type = std::underlying_type_t<T>;
                JsonSerializer<integer_type> integer_serializer;
                return integer_serializer(static_cast<integer_type>(value), json);
            }
        }
    };
}
