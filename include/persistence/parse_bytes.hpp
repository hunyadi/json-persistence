#pragma once
#include "parse_base.hpp"
#include "base64.hpp"

namespace persistence
{
    template<>
    struct JsonParser<byte_vector> : JsonSingleEventHandler<JsonValueString>
    {
        using json_type = JsonValueString;

        JsonParser(ReaderContext& context, byte_vector& ref)
            : JsonSingleEventHandler<JsonValueString>(context)
            , ref(ref)
        {}

        bool parse(const JsonValueString& s) override
        {
            if (base64_decode(s.literal, ref)) {
                context.pop();
                return true;
            } else {
                context.fail("invalid Base64-encoding for sequence of bytes");
                return false;
            }
        }

    private:
        byte_vector& ref;
    };
}
