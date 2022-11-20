#pragma once
#include "enum.hpp"
#include "deserialize_check.hpp"
#include "deserialize_fundamental.hpp"
#include "detail/unlikely.hpp"
#ifdef PERSISTENCE_BOOST_DESCRIBE
#include <boost/describe/enum_from_string.hpp>
#endif

namespace persistence
{
    /** De-serializes an enumeration value from a JSON number. */
    template<bool Exception, typename T, typename Enable = void>
    struct JsonEnumDeserializer : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, T& value) const
        {
            using integer_type = std::underlying_type_t<T>;

            integer_type integer_value;
            PERSISTENCE_IF_UNLIKELY(!deserialize<Exception>(json, integer_value, context)) {
                return false;
            }

            value = static_cast<T>(integer_value);
            return true;
        }
    };

    /** De-serializes an enumeration value from a JSON string. */
    template<bool Exception, typename T>
    struct JsonEnumDeserializer<Exception, T, std::enable_if_t<detect<T, enum_from_string_function>::value>> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, T& value) const
        {
            static_assert(
                std::is_same_v<enum_from_string_function<T>, bool>,
                "expected function with signature: static bool from_string(const std::string_view&, Enum&)"
            );

            if (!detail::check_string<Exception>(json, context)) {
                return false;
            }

            std::string_view view(json.GetString(), json.GetStringLength());
            PERSISTENCE_IF_UNLIKELY(!enum_traits<T>::from_string(view, value)) {
                if constexpr (Exception) {
                    throw JsonDeserializationError(
                        "unrecognized enumeration string value; got: " + std::string(view),
                        Path(context.segments()).str()
                    );
                } else {
                    return false;
                }
            }
            return true;
        }
    };

#ifdef PERSISTENCE_BOOST_DESCRIBE
    /** De-serializes an enumeration value from a JSON string with Boost::Describe. */
    template<bool Exception, typename T>
    struct JsonEnumDeserializer<Exception, T, std::enable_if_t<::boost::describe::has_describe_enumerators<T>::value>> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, T& value) const
        {
            if (!detail::check_string<Exception>(json, context)) {
                return false;
            }

            std::string_view view(json.GetString(), json.GetStringLength());
            PERSISTENCE_IF_UNLIKELY(!::boost::describe::enum_from_string(view.data(), value)) {
                if constexpr (Exception) {
                    throw JsonDeserializationError(
                        "unrecognized enumeration string value; got: " + std::string(view),
                        Path(context.segments()).str()
                    );
                } else {
                    return false;
                }
            }
            return true;
        }
    };
#endif

    template<bool Exception, typename T>
    struct JsonDeserializer<Exception, T, std::enable_if_t<std::is_enum_v<T>>> : JsonEnumDeserializer<Exception, T>
    {
        using JsonEnumDeserializer<Exception, T>::JsonEnumDeserializer;
    };
}
