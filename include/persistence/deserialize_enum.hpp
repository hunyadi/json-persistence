#pragma once
#include "enum.hpp"
#include "deserialize_check.hpp"
#include "deserialize_fundamental.hpp"
#include "detail/unlikely.hpp"

namespace persistence
{
    template<bool Exception, typename T>
    struct JsonDeserializer<Exception, T, typename std::enable_if_t<std::is_enum_v<T>>> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, T& value) const
        {
            if constexpr (detect<T, enum_from_string_function>::value) {
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
                            "unrecognized enumeration value; got: " + std::string(view),
                            Path(context.segments()).str()
                        );
                    } else {
                        return false;
                    }
                }
                return true;
            } else {
                using integer_type = std::underlying_type_t<T>;

                integer_type integer_value;
                PERSISTENCE_IF_UNLIKELY(!deserialize<Exception>(json, integer_value, context)) {
                    return false;
                }

                value = static_cast<T>(integer_value);
                return true;
            }
        }
    };
}
