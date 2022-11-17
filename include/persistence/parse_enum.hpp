#pragma once
#include "enum.hpp"
#include "parse_base.hpp"

namespace persistence
{
    template<typename T>
    struct JsonParser<T, std::enable_if_t<std::is_enum_v<T>>> : JsonEventHandler
    {
        using json_type = JsonValueNumber;

        JsonParser(ReaderContext& context, T& ref)
            : JsonEventHandler(context)
            , ref(ref)
        {}

        bool parse(const JsonValueString& s) override
        {
            if constexpr (detect<T, enum_from_string_function>::value) {
                static_assert(
                    std::is_same_v<enum_from_string_function<T>, bool>,
                    "expected function with signature: static bool from_string(const std::string_view&, Enum&)"
                );

                if (enum_traits<T>::from_string(s.literal, ref)) {
                    return true;
                } else {
                    context.fail("expected an enumeration string value; got: " + std::string(s.literal.data(), s.literal.size()));
                    return false;
                }
            } else {
                context.fail("expected an enumeration numeric value; got: " + std::string(s.literal.data(), s.literal.size()));
                return false;
            }
        }

        bool parse(const JsonValueNumber& n) override
        {
            if constexpr (!detect<T, enum_from_string_function>::value) {
                std::underlying_type_t<T> value;
                const char* last = n.literal.data() + n.literal.size();
                auto result = std::from_chars(n.literal.data(), last, value);
                if (result.ec == std::errc() && result.ptr == last) {
                    ref = static_cast<T>(value);
                    context.pop();
                    return true;
                } else {
                    context.fail("expected an enumeration numeric value; got: " + std::string(n.literal.data(), n.literal.size()));
                    return false;
                }
            } else {
                context.fail("expected an enumeration string value; got: " + std::string(n.literal.data(), n.literal.size()));
                return false;
            }
        }

    private:
        T& ref;
    };
}
