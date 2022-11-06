#pragma once
#if __cplusplus < 202002L
#error This feature requires C++20 or later.
#endif
#include "datetime.hpp"
#include "write_base.hpp"

namespace persistence
{
    template<>
    struct JsonWriter<std::chrono::year_month_day>
    {
        bool operator()(std::chrono::year_month_day value, StringWriter& writer) const
        {
            auto dt = to_iso_date(std::chrono::sys_days(value));
            writer.String(dt.text, static_cast<rapidjson::SizeType>(iso_date::length()), true);
            return true;
        }
    };
}
