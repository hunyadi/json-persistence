#pragma once
#include "number.hpp"
#include "parse_base.hpp"
#include "detail/unlikely.hpp"

namespace persistence
{
    template<>
    struct JsonParser<std::nullptr_t> : JsonSingleEventHandler<JsonValueNull>
    {
        using json_type = JsonValueNull;

        JsonParser(ReaderContext& context, std::nullptr_t& ref)
            : JsonSingleEventHandler<JsonValueNull>(context)
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
    struct JsonParser<bool> : JsonSingleEventHandler<JsonValueBoolean>
    {
        using json_type = JsonValueBoolean;

        JsonParser(ReaderContext& context, bool& ref)
            : JsonSingleEventHandler<JsonValueBoolean>(context)
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
        static bool parse(ReaderContext& context, const JsonValueNumber& n, T& ref)
        {
            if (parse_number(n.literal, ref)) {
                return true;
            } else {
                auto value = std::string(n.literal.data(), n.literal.size());
                if constexpr (std::is_integral_v<T>) {
                    context.fail("expected an integer; got: " + value);
                } else if constexpr (std::is_floating_point_v<T>) {
                    context.fail("expected a floating-point number; got: " + value);
                }
                return false;
            }
        }
    };

    template<typename T>
    struct JsonNumberParser : JsonSingleEventHandler<JsonValueNumber>
    {
        using json_type = JsonValueNumber;

        JsonNumberParser(ReaderContext& context, T& ref)
            : JsonSingleEventHandler<JsonValueNumber>(context)
            , ref(ref)
        {}

        bool parse(const JsonValueNumber& n) override
        {
            if (JsonNumberValueParser<T>::parse(context, n, ref)) {
                context.pop();
                return true;
            } else {
                return false;
            }
        }

    private:
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

    template<>
    struct JsonParser<std::string> : JsonSingleEventHandler<JsonValueString>
    {
        using json_type = JsonValueString;

        JsonParser(ReaderContext& context, std::string& ref)
            : JsonSingleEventHandler<JsonValueString>(context)
            , ref(ref)
        {}

        bool parse(const JsonValueString& s) override
        {
            ref.assign(s.literal.begin(), s.literal.end());
            context.pop();
            return true;
        }

    private:
        std::string& ref;
    };
}
