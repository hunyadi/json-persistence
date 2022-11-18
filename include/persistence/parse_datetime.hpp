#pragma once
#include "datetime.hpp"
#include "parse_base.hpp"

namespace persistence
{
    template<>
    struct JsonParser<timestamp> : JsonSingleEventHandler<JsonValueString>
    {
        using json_type = JsonValueString;

        JsonParser(ReaderContext& context, timestamp& ref)
            : JsonSingleEventHandler<JsonValueString>(context)
            , ref(ref)
        {}

        bool parse(const JsonValueString& s) override
        {
            if (parse_datetime(s.literal.data(), s.literal.size(), ref)) {
                context.pop();
                return true;
            } else {
                context.fail("invalid ISO-8601 date-time; expected: YYYY-MM-DDTHH:MM:SSZ, got: " + std::string(s.literal.data(), s.literal.size()));
                return false;
            }
        }

    private:
        timestamp& ref;
    };
}
