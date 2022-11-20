#pragma once
#include "enum.hpp"
#include "serialize_fundamental.hpp"
#include "serialize_string.hpp"
#ifdef PERSISTENCE_BOOST_DESCRIBE
#include <boost/describe/enum_to_string.hpp>
#endif

namespace persistence
{
    /** Serializes an enumeration value as a JSON number. */
    template<typename T, typename Enable = void>
    struct JsonEnumSerializer
    {
        bool operator()(T value, rapidjson::Value& json) const
        {
            using integer_type = std::underlying_type_t<T>;
            JsonSerializer<integer_type> integer_serializer;
            return integer_serializer(static_cast<integer_type>(value), json);
        }
    };

    template<typename T>
    struct JsonEnumSerializer<T, std::enable_if_t<detect<T, enum_to_string_function>::value>>
    {
        bool operator()(T value, rapidjson::Value& json) const
        {
            static_assert(
                std::is_same_v<enum_to_string_function<T>, std::string_view>,
                "expected function with signature: static std::string_view to_string(Enum)"
            );
            JsonSerializer<std::string_view> string_serializer;
            return string_serializer(enum_traits<T>::to_string(value), json);
        }
    };

#ifdef PERSISTENCE_BOOST_DESCRIBE
    template<typename T>
    struct JsonEnumSerializer<T, std::enable_if_t<::boost::describe::has_describe_enumerators<T>::value>>
    {
        bool operator()(T value, rapidjson::Value& json) const
        {
            JsonSerializer<std::string_view> string_serializer;
            return string_serializer(::boost::describe::enum_to_string(value, ""), json);
        }
    };
#endif

    template<typename T>
    struct JsonSerializer<T, std::enable_if_t<std::is_enum_v<T>>> : JsonEnumSerializer<T>
    {};
}
