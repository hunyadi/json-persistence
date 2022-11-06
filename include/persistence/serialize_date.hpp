#pragma once
#if __cplusplus < 202002L
#error This feature requires C++20 or later.
#endif
#include "datetime.hpp"
#include "serialize_base.hpp"
#include "detail/serialize_aware.hpp"

namespace persistence
{
    template<>
    struct JsonSerializer<std::chrono::year_month_day> : JsonContextAwareSerializer
    {
        using JsonContextAwareSerializer::JsonContextAwareSerializer;

        bool operator()(std::chrono::year_month_day value, rapidjson::Value& json) const
        {
            auto dt = to_iso_date(std::chrono::sys_days(value));
            json.SetString(dt.text, static_cast<rapidjson::SizeType>(iso_date::length()), context.global().allocator());
            return true;
        }
    };
}
