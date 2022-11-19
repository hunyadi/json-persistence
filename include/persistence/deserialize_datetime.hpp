#pragma once
#include "datetime.hpp"
#include "deserialize_base.hpp"
#include "deserialize_check.hpp"
#include "detail/deserialize_aware.hpp"
#include "detail/unlikely.hpp"

namespace persistence
{
    template<bool Exception>
    struct JsonDeserializer<Exception, timestamp> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, timestamp& value) const
        {
            if (!detail::check_string<Exception>(json, context)) {
                return false;
            }

            PERSISTENCE_IF_UNLIKELY(!parse_datetime(json.GetString(), json.GetStringLength(), value)) {
                if constexpr (Exception) {
                    throw JsonDeserializationError(
                        "invalid ISO-8601 date-time; expected: YYYY-MM-DDTHH:MM:SSZ, got: " + std::string(json.GetString(), json.GetStringLength()),
                        Path(context.segments()).str()
                    );
                } else {
                    return false;
                }
            }

            return true;
        }
    };
}
