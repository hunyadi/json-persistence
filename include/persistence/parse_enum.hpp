#pragma once
#include "enum.hpp"
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

        bool parse(JsonValueString s) override
        {
            if constexpr (detect<T, enum_from_string_function>::value) {
                static_assert(
                    std::is_same_v<enum_from_string_function<T>, bool>,
                    "expected function with signature: static bool from_string(const std::string_view&, Enum&)"
                );

                return enum_traits<T>::from_string(s.literal, ref);
            } else {
                return false;
            }
        }

        bool parse(JsonValueNumber n) override
        {
            if constexpr (!detect<T, enum_from_string_function>::value) {
                typename std::underlying_type<T>::type value;
                const char* last = n.literal.data() + n.literal.size();
                auto result = std::from_chars(n.literal.data(), last, value);
                if (result.ec != std::errc() || result.ptr != last) {
                    return false;
                }

                ref = static_cast<T>(value);
                context.pop();
                return true;
            } else {
                return false;
            }
        }

    private:
        ReaderContext& context;
        T& ref;
    };
}
