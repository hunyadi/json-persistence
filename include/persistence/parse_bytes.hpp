#pragma once
#include "parse_base.hpp"
#include "base64.hpp"

namespace persistence
{
    template<>
    struct JsonParser<byte_vector> : EventHandler
    {
        using json_type = JsonValueString;

        JsonParser(ReaderContext& context, byte_vector& ref)
            : context(context)
            , ref(ref)
        {}

        bool parse(JsonValueString s) override
        {
            if (!base64_decode(s.literal, ref)) {
                return false;
            }

            context.pop();
            return true;
        }

    private:
        ReaderContext& context;
        byte_vector& ref;
    };
}
