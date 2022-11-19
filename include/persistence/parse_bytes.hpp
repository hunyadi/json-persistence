#pragma once
#include "base64.hpp"
#include "parse_base.hpp"

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
            PERSISTENCE_IF_UNLIKELY(!base64_decode(s.literal, ref)) {
                context.fail("invalid Base64-encoding for sequence of bytes");
                return false;
            }

            context.pop();
            return true;
        }

    private:
        byte_vector& ref;
    };
}
