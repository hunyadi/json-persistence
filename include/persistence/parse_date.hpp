#pragma once
#if __cplusplus < 202002L
#error This feature requires C++20 or later.
#endif
#include "parse_base.hpp"
#include "datetime.hpp"

namespace persistence
{
    template<>
    struct JsonParser<std::chrono::year_month_day> : JsonSingleEventHandler<JsonValueString>
    {
        using json_type = JsonValueString;

        JsonParser(ReaderContext& context, std::chrono::year_month_day& ref)
            : JsonSingleEventHandler<JsonValueString>(context)
            , ref(ref)
        {}

        bool parse(const JsonValueString& s) override
        {
            timestamp ts;
            if (parse_date(s.literal.data(), s.literal.size(), ts)) {
                ref = std::chrono::time_point_cast<std::chrono::days>(ts);
                context.pop();
                return true;
            } else {
                context.fail("invalid ISO-8601 date; expected: YYYY-MM-DD, got: " + std::string(s.literal.data(), s.literal.size()));
                return false;
            }
        }

    private:
        std::chrono::year_month_day& ref;
    };
}
