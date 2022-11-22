#pragma once
#include "parse_base.hpp"
#include <string>

namespace persistence
{
    template<>
    struct JsonParser<std::string> : JsonParseHandler<JsonValueString>
    {
        using json_type = JsonValueString;

        JsonParser(ReaderContext& context, std::string& ref)
            : JsonParseHandler(context)
            , ref(ref)
        {}

        bool parse(const JsonValueString& s) override
        {
            ref = s.literal;
            context.pop();
            return true;
        }

    private:
        std::string& ref;
    };
}
