#pragma once
#include "parse_base.hpp"
#include "datetime.hpp"
#include <charconv>

namespace persistence
{
    template<>
    struct JsonParser<timestamp> : EventHandler
    {
        using json_type = JsonValueString;

        JsonParser(ReaderContext& context, timestamp& ref)
            : context(context)
            , ref(ref)
        {}

        bool parse(JsonValueString s) override
        {
            bool result = parse_datetime(s.literal.data(), s.literal.size(), ref);
            context.pop();
            return result;
        }

    private:
        ReaderContext& context;
        timestamp& ref;
    };
}
