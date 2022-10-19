#pragma once
#include "read_base.hpp"
#include <set>

namespace persistence
{
    /**
     * Parses a JSON array of possibly composite values into a C++ `set<T>`.
     */
    template<typename T>
    struct JsonSetParser : EventHandler
    {};

    /**
     * Parses a JSON array of numbers into a C++ `set<T>` efficiently.
     *
     * @tparam Integer or floating-point type.
     */
    template<typename T>
    struct JsonNumberSetParser : EventHandler
    {
        static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");

        JsonNumberSetParser(ReaderContext& context, std::set<T>& container)
            : context(context)
            , container(container)
        {}

        bool parse(JsonArrayEnd) override
        {
            context.pop();
            return true;
        }

        bool parse(JsonValueNumber n) override
        {
            T value;
            if (!parse_number(n.literal, value)) {
                return false;
            }

            container.insert(value);
            return true;
        }

    private:
        ReaderContext& context;
        std::set<T>& container;
    };

    template<>
    struct JsonSetParser<short> : JsonNumberSetParser<short>
    {
        using JsonNumberSetParser<short>::JsonNumberSetParser;
    };

    template<>
    struct JsonSetParser<unsigned short> : JsonNumberSetParser<unsigned short>
    {
        using JsonNumberSetParser<unsigned short>::JsonNumberSetParser;
    };

    template<>
    struct JsonSetParser<int> : JsonNumberSetParser<int>
    {
        using JsonNumberSetParser<int>::JsonNumberSetParser;
    };

    template<>
    struct JsonSetParser<unsigned int> : JsonNumberSetParser<unsigned int>
    {
        using JsonNumberSetParser<unsigned int>::JsonNumberSetParser;
    };

    template<>
    struct JsonSetParser<long> : JsonNumberSetParser<long>
    {
        using JsonNumberSetParser<long>::JsonNumberSetParser;
    };

    template<>
    struct JsonSetParser<unsigned long> : JsonNumberSetParser<unsigned long>
    {
        using JsonNumberSetParser<unsigned long>::JsonNumberSetParser;
    };

    template<>
    struct JsonSetParser<long long> : JsonNumberSetParser<long long>
    {
        using JsonNumberSetParser<long long>::JsonNumberSetParser;
    };

    template<>
    struct JsonSetParser<unsigned long long> : JsonNumberSetParser<unsigned long long>
    {
        using JsonNumberSetParser<unsigned long long>::JsonNumberSetParser;
    };

    template<>
    struct JsonSetParser<float> : JsonNumberSetParser<float>
    {
        using JsonNumberSetParser<float>::JsonNumberSetParser;
    };

    template<>
    struct JsonSetParser<double> : JsonNumberSetParser<double>
    {
        using JsonNumberSetParser<double>::JsonNumberSetParser;
    };

    template<>
    struct JsonSetParser<std::string> : EventHandler
    {
        JsonSetParser(ReaderContext& context, std::set<std::string>& container)
            : context(context)
            , container(container)
        {}

        bool parse(JsonArrayEnd) override
        {
            context.pop();
            return true;
        }

        bool parse(JsonValueString s) override
        {
            container.emplace(s.literal.begin(), s.literal.end());
            return true;
        }

    private:
        ReaderContext& context;
        std::set<std::string>& container;
    };

    template<typename T>
    struct JsonParser<std::set<T>> : EventHandler
    {
        using json_type = JsonArrayStart;

        JsonParser(ReaderContext& context, std::set<T>& ref)
            : context(context)
            , ref(ref)
        {}

        bool parse(JsonArrayStart) override
        {
            context.replace(std::make_unique<JsonSetParser<T>>(context, ref));
            return true;
        }

    private:
        ReaderContext& context;
        std::set<T>& ref;
    };
}
