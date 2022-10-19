#pragma once
#include "deserialize_base.hpp"
#include "datetime.hpp"
#include <charconv>
#include <chrono>
#include <string>

namespace persistence
{
    template<>
    struct JsonDeserializer<timestamp>
    {
        bool operator()(const rapidjson::Value& json, timestamp& value) const
        {
            if (!json.IsString()) {
                return false;
            }

            return parse_datetime(json.GetString(), json.GetStringLength(), value);
        }
    };
}
