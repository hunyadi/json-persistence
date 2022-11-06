#pragma once
#include "datetime.hpp"
#include "serialize_base.hpp"
#include "detail/serialize_aware.hpp"

namespace persistence
{
    template<>
    struct JsonSerializer<timestamp> : JsonContextAwareSerializer
    {
        using JsonContextAwareSerializer::JsonContextAwareSerializer;

        bool operator()(timestamp value, rapidjson::Value& json) const
        {
            auto dt = to_iso_datetime(value);
            json.SetString(dt.text, static_cast<rapidjson::SizeType>(iso_datetime::length()), context.global().allocator());
            return true;
        }
    };
}
