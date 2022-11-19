#pragma once
#if __cplusplus < 202002L
#error This feature requires C++20 or later.
#endif
#include "datetime.hpp"
#include "deserialize_base.hpp"
#include "deserialize_check.hpp"
#include "detail/deserialize_aware.hpp"
#include "detail/unlikely.hpp"

namespace persistence
{
    template<bool Exception>
    struct JsonDeserializer<Exception, std::chrono::year_month_day> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, std::chrono::year_month_day& value) const
        {
            if (!detail::check_string<Exception>(json, context)) {
                return false;
            }

            timestamp ts;
            PERSISTENCE_IF_UNLIKELY(!parse_date(json.GetString(), json.GetStringLength(), ts)) {
                if constexpr (Exception) {
                    throw JsonDeserializationError(
                        "invalid ISO-8601 date; expected: YYYY-MM-DD, got: " + std::string(json.GetString(), json.GetStringLength()),
                        Path(context.segments()).str()
                    );
                } else {
                    return false;
                }
            }

            value = std::chrono::time_point_cast<std::chrono::days>(ts);
            return true;
        }
    };
}
