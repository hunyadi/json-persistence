#pragma once
#if __cplusplus < 202002L
#error This feature requires C++20 or later.
#endif
#include "parse_base.hpp"
#include "datetime.hpp"

namespace persistence
{
    template<>
    struct JsonParser<std::chrono::year_month_day> : EventHandler
    {
        using json_type = JsonValueString;

        JsonParser(ReaderContext& context, std::chrono::year_month_day& ref)
            : context(context)
            , ref(ref)
        {}

        bool parse(const JsonValueString& s) override
        {
            timestamp ts;
            bool result = parse_date(s.literal.data(), s.literal.size(), ts);
            ref = std::chrono::time_point_cast<std::chrono::days>(ts);
            context.pop();
            return result;
        }

    private:
        ReaderContext& context;
        std::chrono::year_month_day& ref;
    };
}
