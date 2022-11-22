#pragma once
#if __cplusplus < 202002L
#error This feature requires C++20 or later.
#endif
#include "datetime.hpp"
#include "parse_base.hpp"
#include "detail/unlikely.hpp"

namespace persistence
{
    template<>
    struct JsonParser<std::chrono::year_month_day> : JsonParseHandler<JsonValueString>
    {
        using json_type = JsonValueString;

        JsonParser(ReaderContext& context, std::chrono::year_month_day& ref)
            : JsonParseHandler(context)
            , ref(ref)
        {}

        bool parse(const JsonValueString& s) override
        {
            timestamp ts;
            PERSISTENCE_IF_UNLIKELY(!parse_date(s.literal.data(), s.literal.size(), ts)) {
                context.fail("invalid ISO-8601 date; expected: YYYY-MM-DD, got: " + std::string(s.literal));
                return false;
            }

            ref = std::chrono::time_point_cast<std::chrono::days>(ts);
            context.pop();
            return true;
        }

    private:
        std::chrono::year_month_day& ref;
    };
}
