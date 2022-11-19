#pragma once
#include "enum.hpp"
#include "parse_base.hpp"
#include "parse_fundamental.hpp"
#include "detail/traits.hpp"
#include "detail/unlikely.hpp"
#include <charconv>

namespace persistence
{
    template<typename T>
    struct JsonParser<T, std::enable_if_t<std::is_enum_v<T>>> : JsonParseHandler
    {
        using json_type = JsonValueNumber;

        JsonParser(ReaderContext& context, T& ref)
            : JsonParseHandler(context)
            , ref(ref)
        {}

        bool parse(const JsonValueString& s) override
        {
            if constexpr (detect<T, enum_from_string_function>::value) {
                static_assert(
                    std::is_same_v<enum_from_string_function<T>, bool>,
                    "expected function with signature: static bool from_string(const std::string_view&, Enum&)"
                );

                PERSISTENCE_IF_UNLIKELY(!enum_traits<T>::from_string(s.literal, ref)) {
                    context.fail("expected an enumeration string value; got: " + std::string(s.literal.data(), s.literal.size()));
                    return false;
                }

                return true;
            } else {
                context.fail("expected an enumeration numeric value; got: " + std::string(s.literal.data(), s.literal.size()));
                return false;
            }
        }

        bool parse(const JsonValueInteger& n) override
        {
            return parse_integer(n.value);
        }

        bool parse(const JsonValueUnsigned& n) override
        {
            return parse_integer(n.value);
        }

        bool parse(const JsonValueInteger64& n) override
        {
            return parse_integer(n.value);
        }

        bool parse(const JsonValueUnsigned64& n) override
        {
            return parse_integer(n.value);
        }

        bool parse(const JsonValueNumber& n) override
        {
            if constexpr (!detect<T, enum_from_string_function>::value) {
                std::underlying_type_t<T> value;
                const char* last = n.literal.data() + n.literal.size();
                auto result = std::from_chars(n.literal.data(), last, value);

                PERSISTENCE_IF_UNLIKELY(result.ec != std::errc() || result.ptr != last) {
                    context.fail("expected an enumeration numeric value; got: " + std::string(n.literal.data(), n.literal.size()));
                    return false;
                }

                ref = static_cast<T>(value);
                context.pop();
                return true;
            } else {
                context.fail("expected an enumeration string value; got: " + std::string(n.literal.data(), n.literal.size()));
                return false;
            }
        }

    private:
        template<typename V>
        bool parse_integer(V value)
        {
            if constexpr (!detect<T, enum_from_string_function>::value) {
                using integer_type = std::underlying_type_t<T>;
                integer_type integer_value;
                PERSISTENCE_IF_UNLIKELY(!JsonNumberValueParser<integer_type>::parse(context, value, integer_value)) {
                    return false;
                }

                ref = static_cast<T>(integer_value);
                context.pop();
                return true;
            } else {
                context.fail("expected an enumeration string value; got: " + std::to_string(value));
                return false;
            }
        }

    private:
        T& ref;
    };
}
