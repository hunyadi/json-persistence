#pragma once
#include "serialize_fundamental.hpp"

namespace persistence
{
    template<typename T>
    struct JsonSerializer<T, typename std::enable_if<std::is_enum<T>::value>::type>
    {
        bool operator()(T value, rapidjson::Value& json) const
        {
            using integer_type = typename std::underlying_type<T>::type;
            JsonSerializer<integer_type> integer_serializer;
            return integer_serializer(static_cast<integer_type>(value), json);
        }
    };
}
