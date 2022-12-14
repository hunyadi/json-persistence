#pragma once
#include "base64.hpp"
#include "deserialize_base.hpp"
#include "deserialize_check.hpp"
#include "detail/deserialize_aware.hpp"
#include "detail/unlikely.hpp"

namespace persistence
{
    template<bool Exception>
    struct JsonDeserializer<Exception, byte_vector> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, byte_vector& container) const
        {
            if (!detail::check_string<Exception>(json, context)) {
                return false;
            }

            PERSISTENCE_IF_UNLIKELY(!base64_decode(std::string_view(json.GetString(), json.GetStringLength()), container)) {
                if constexpr (Exception) {
                    throw JsonDeserializationError(
                        "invalid Base64-encoding for sequence of bytes",
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
