#pragma once
#include "deserialize_base.hpp"
#include "detail/engine.hpp"
#include <string>

namespace persistence
{
    template<>
    struct JsonDeserializer<std::string>
    {
        bool operator()(const rapidjson::Value& json, std::string& value) const
        {
            if (json.IsString()) {
                value = json.GetString();
                return true;
            } else {
                return false;
            }
        }
    };
}
