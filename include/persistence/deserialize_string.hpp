#pragma once
#include "deserialize_base.hpp"
#include "deserialize_check.hpp"
#include "detail/deserialize_aware.hpp"
#include <string>

namespace persistence
{
    template<bool Exception>
    struct JsonDeserializer<Exception, std::string> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, std::string& value) const
        {
            if (!detail::check_string<Exception>(json, context)) {
                return false;
            }

            value.assign(json.GetString(), json.GetStringLength());
            return true;
        }
    };
}
