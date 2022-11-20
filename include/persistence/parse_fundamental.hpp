#pragma once
#include "number.hpp"
#include "parse_base.hpp"
#include "detail/numeric_traits.hpp"
#include "detail/unlikely.hpp"

namespace persistence
{
    template<>
    struct JsonParser<std::nullptr_t> : JsonParseHandler<JsonValueNull>
    {
        using json_type = JsonValueNull;

        JsonParser(ReaderContext& context, std::nullptr_t& ref)
            : JsonParseHandler(context)
            , ref(ref)
        {}

        bool parse(const JsonValueNull&) override
        {
            ref = nullptr;
            context.pop();
            return true;
        }

    private:
        std::nullptr_t& ref;
    };

    template<>
    struct JsonParser<bool> : JsonParseHandler<JsonValueBoolean>
    {
        using json_type = JsonValueBoolean;

        JsonParser(ReaderContext& context, bool& ref)
            : JsonParseHandler(context)
            , ref(ref)
        {}

        bool parse(const JsonValueBoolean& b) override
        {
            ref = b.value;
            context.pop();
            return true;
        }

    private:
        bool& ref;
    };

    template<typename T>
    struct JsonNumberValueParser
    {
        template<typename V>
        static bool parse(ReaderContext& context, V value, T& ref)
        {
            PERSISTENCE_IF_UNLIKELY(!is_assignable<T>(value)) {
                context.fail("number cannot be assigned without data loss");
                return false;
            }

            ref = static_cast<T>(value);
            return true;
        }

        static bool parse(ReaderContext& context, const std::string_view& literal, T& ref)
        {
            PERSISTENCE_IF_UNLIKELY(!parse_number(literal, ref)) {
                auto value = std::string(literal.data(), literal.size());
                if constexpr (std::is_integral_v<T>) {
                    context.fail("expected an integer; got: " + value);
                } else if constexpr (std::is_floating_point_v<T>) {
                    context.fail("expected a floating-point number; got: " + value);
                }
                return false;
            }

            return true;
        }
    };

    template<typename T>
    struct JsonNumberParser : JsonParseHandler<JsonValueNumber>
    {
        using json_type = JsonValueNumber;

        JsonNumberParser(ReaderContext& context, T& ref)
            : JsonParseHandler(context)
            , ref(ref)
        {}

        bool parse(const JsonValueInteger& n) override
        {
            return parse_number(n.value);
        }

        bool parse(const JsonValueUnsigned& n) override
        {
            return parse_number(n.value);
        }

        bool parse(const JsonValueInteger64& n) override
        {
            return parse_number(n.value);
        }

        bool parse(const JsonValueUnsigned64& n) override
        {
            return parse_number(n.value);
        }

        bool parse(const JsonValueDouble& n) override
        {
            return parse_number(n.value);
        }

        bool parse(const JsonValueNumber& n) override
        {
            PERSISTENCE_IF_UNLIKELY(!JsonNumberValueParser<T>::parse(context, n.literal, ref)) {
                return false;
            }

            context.pop();
            return true;
        }

    private:
        template<typename V>
        bool parse_number(V value)
        {
            PERSISTENCE_IF_UNLIKELY(!JsonNumberValueParser<T>::parse(context, value, ref)) {
                return false;
            }

            context.pop();
            return true;
        }

    protected:
        T& ref;
    };

    template<>
    struct JsonParser<char> : JsonNumberParser<char>
    {
        using JsonNumberParser<char>::JsonNumberParser;
    };

    template<>
    struct JsonParser<signed char> : JsonNumberParser<signed char>
    {
        using JsonNumberParser<signed char>::JsonNumberParser;
    };

    template<>
    struct JsonParser<unsigned char> : JsonNumberParser<unsigned char>
    {
        using JsonNumberParser<unsigned char>::JsonNumberParser;
    };

    template<>
    struct JsonParser<short> : JsonNumberParser<short>
    {
        using JsonNumberParser<short>::JsonNumberParser;
    };

    template<>
    struct JsonParser<unsigned short> : JsonNumberParser<unsigned short>
    {
        using JsonNumberParser<unsigned short>::JsonNumberParser;
    };

    template<>
    struct JsonParser<int> : JsonNumberParser<int>
    {
        using JsonNumberParser<int>::JsonNumberParser;
    };

    template<>
    struct JsonParser<unsigned int> : JsonNumberParser<unsigned int>
    {
        using JsonNumberParser<unsigned int>::JsonNumberParser;
    };

    template<>
    struct JsonParser<long> : JsonNumberParser<long>
    {
        using JsonNumberParser<long>::JsonNumberParser;
    };

    template<>
    struct JsonParser<unsigned long> : JsonNumberParser<unsigned long>
    {
        using JsonNumberParser<unsigned long>::JsonNumberParser;
    };

    template<>
    struct JsonParser<long long> : JsonNumberParser<long long>
    {
        using JsonNumberParser<long long>::JsonNumberParser;
    };

    template<>
    struct JsonParser<unsigned long long> : JsonNumberParser<unsigned long long>
    {
        using JsonNumberParser<unsigned long long>::JsonNumberParser;
    };

    template<>
    struct JsonParser<float> : JsonNumberParser<float>
    {
        using JsonNumberParser<float>::JsonNumberParser;
    };

    template<>
    struct JsonParser<double> : JsonNumberParser<double>
    {
        using JsonNumberParser<double>::JsonNumberParser;
    };
}
