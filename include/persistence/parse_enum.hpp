#pragma once
#include "parse_base.hpp"

namespace persistence
{
    template<typename T>
    struct JsonParser<T, typename std::enable_if<std::is_enum<T>::value>::type> : EventHandler
    {
        using json_type = JsonValueNumber;

        JsonParser(ReaderContext& context, T& ref)
            : context(context)
            , ref(ref)
        {}

        bool parse(JsonValueNumber n) override
        {
            typename std::underlying_type<T>::type value;
            const char* last = n.literal.data() + n.literal.size();
            auto result = std::from_chars(n.literal.data(), last, value);
            if (result.ec != std::errc() || result.ptr != last) {
                return false;
            }

            ref = static_cast<T>(value);
            context.pop();
            return true;
        }

    private:
        ReaderContext& context;
        T& ref;
    };
}
