#pragma once
#include "deserialize_base.hpp"
#include "base64.hpp"

namespace persistence
{
    template<>
    struct JsonDeserializer<byte_vector>
    {
        bool operator()(const rapidjson::Value& json, byte_vector& container) const
        {
            if (!json.IsString()) {
                return false;
            }

            return base64_decode(std::string_view(json.GetString(), json.GetStringLength()), container);
        }
    };
}
