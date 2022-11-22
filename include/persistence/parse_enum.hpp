#pragma once
#include "enum.hpp"
#include "parse_base.hpp"
#include "parse_fundamental.hpp"
#include "detail/traits.hpp"
#include "detail/unlikely.hpp"
#include <charconv>
#ifdef PERSISTENCE_BOOST_DESCRIBE
#include <boost/describe/enum_from_string.hpp>
#endif

namespace persistence
{
    template<typename T, typename Enable = void>
    struct JsonEnumParser : JsonParseHandler<JsonValueNumber>
    {
        using json_type = JsonValueNumber;

        JsonEnumParser(ReaderContext& context, T& ref)
            : JsonParseHandler(context)
            , ref(ref)
        {}

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
            std::underlying_type_t<T> value;
            const char* last = n.literal.data() + n.literal.size();
            auto result = std::from_chars(n.literal.data(), last, value);

            PERSISTENCE_IF_UNLIKELY(result.ec != std::errc() || result.ptr != last) {
                context.fail("expected an enumeration numeric value; got: " + std::string(n.literal));
                return false;
            }

            ref = static_cast<T>(value);
            context.pop();
            return true;
        }

    private:
        template<typename V>
        bool parse_integer(V value)
        {
            using integer_type = std::underlying_type_t<T>;
            integer_type integer_value;
            PERSISTENCE_IF_UNLIKELY(!JsonNumberValueParser<integer_type>::parse(context, value, integer_value)) {
                return false;
            }

            ref = static_cast<T>(integer_value);
            context.pop();
            return true;
        }

    private:
        T& ref;
    };

    template<typename T>
    struct JsonEnumParser<T, std::enable_if_t<detect<T, enum_from_string_function>::value>> : JsonParseHandler<JsonValueString>
    {
        using json_type = JsonValueString;

        JsonEnumParser(ReaderContext& context, T& ref)
            : JsonParseHandler(context)
            , ref(ref)
        {}

        bool parse(const JsonValueString& s) override
        {
            static_assert(
                std::is_same_v<enum_from_string_function<T>, bool>,
                "expected function with signature: static bool from_string(const std::string_view&, Enum&)"
            );

            PERSISTENCE_IF_UNLIKELY(!enum_traits<T>::from_string(s.literal, ref)) {
                context.fail("expected an enumeration string value; got: " + std::string(s.literal));
                return false;
            }

            return true;
        }

    private:
        T& ref;
    };

#ifdef PERSISTENCE_BOOST_DESCRIBE
    template<typename T>
    struct JsonEnumParser<T, std::enable_if_t<::boost::describe::has_describe_enumerators<T>::value>> : JsonParseHandler<JsonValueString>
    {
        using json_type = JsonValueString;

        JsonEnumParser(ReaderContext& context, T& ref)
            : JsonParseHandler(context)
            , ref(ref)
        {}

        bool parse(const JsonValueString& s) override
        {
            PERSISTENCE_IF_UNLIKELY(!::boost::describe::enum_from_string(s.literal.data(), ref)) {
                context.fail("expected an enumeration string value; got: " + std::string(s.literal));
                return false;
            }

            return true;
        }

    private:
        T& ref;
    };
#endif

    template<typename T>
    struct JsonParser<T, std::enable_if_t<std::is_enum_v<T>>> : JsonEnumParser<T>
    {
        using JsonEnumParser<T>::JsonEnumParser;
    };
}
