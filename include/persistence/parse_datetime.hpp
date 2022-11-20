#pragma once
#include "datetime.hpp"
#include "parse_base.hpp"
#include "detail/unlikely.hpp"

namespace persistence
{
    template<>
    struct JsonParser<timestamp> : JsonParseHandler<JsonValueString>
    {
        using json_type = JsonValueString;

        JsonParser(ReaderContext& context, timestamp& ref)
            : JsonParseHandler(context)
            , ref(ref)
        {}

        bool parse(const JsonValueString& s) override
        {
            PERSISTENCE_IF_UNLIKELY(!parse_datetime(s.literal.data(), s.literal.size(), ref)) {
                context.fail("invalid ISO-8601 date-time; expected: YYYY-MM-DDTHH:MM:SSZ, got: " + std::string(s.literal.data(), s.literal.size()));
                return false;
            }

            context.pop();
            return true;
        }

    private:
        timestamp& ref;
    };
}
