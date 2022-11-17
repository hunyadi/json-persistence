#pragma once
#include "number.hpp"
#include "parse_base.hpp"
#include "detail/unlikely.hpp"

namespace persistence
{
    template<>
    struct JsonParser<std::nullptr_t> : EventHandler
    {
        using json_type = JsonValueNull;

        JsonParser(ReaderContext& context, std::nullptr_t& ref)
            : context(context)
            , ref(ref)
        {}

        bool parse(const JsonValueNull&) override
        {
            ref = nullptr;
            context.pop();
            return true;
        }

    private:
        ReaderContext& context;
        std::nullptr_t& ref;
    };

    template<>
    struct JsonParser<bool> : EventHandler
    {
        using json_type = JsonValueBoolean;

        JsonParser(ReaderContext& context, bool& ref)
            : context(context)
            , ref(ref)
        {}

        bool parse(const JsonValueBoolean& b) override
        {
            ref = b.value;
            context.pop();
            return true;
        }

    private:
        ReaderContext& context;
        bool& ref;
    };

    template<typename T>
    struct JsonNumberParser : EventHandler
    {
        using json_type = JsonValueNumber;

        JsonNumberParser(ReaderContext& context, T& ref)
            : context(context)
            , ref(ref)
        {}

        bool parse(const JsonValueNumber& n) override
        {
            PERSISTENCE_IF_UNLIKELY(!parse_number(n.literal, ref)) {
                return false;
            }

            context.pop();
            return true;
        }

    private:
        ReaderContext& context;
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
    struct JsonParser<std::string> : EventHandler
    {
        using json_type = JsonValueString;

        JsonParser(ReaderContext& context, std::string& ref)
            : context(context)
            , ref(ref)
        {}

        bool parse(const JsonValueString& s) override
        {
            ref.assign(s.literal.begin(), s.literal.end());
            context.pop();
            return true;
        }

    private:
        ReaderContext& context;
        std::string& ref;
    };
}
