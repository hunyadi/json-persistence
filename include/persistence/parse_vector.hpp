#pragma once
#include "parse_base.hpp"

namespace persistence
{
    /**
     * Parses a JSON array of possibly composite values into a C++ `vector<T>`.
     */
    template<typename T>
    struct JsonArrayParser : EventHandler
    {
        JsonArrayParser(ReaderContext& context, std::vector<T>& container)
            : context(context)
            , container(container)
        {}

        bool parse(JsonArrayEnd) override
        {
            context.pop();
            return true;
        }

        bool parse(typename JsonParser<T>::json_type json_item) override
        {
            container.emplace_back();
            auto&& handler = context.push(std::make_unique<JsonParser<T>>(context, container.back()));
            return handler.parse(json_item);
        }

    private:
        ReaderContext& context;
        std::vector<T>& container;
    };

    /**
     * Parses a JSON array of boolean values into a C++ `vector<bool>` efficiently.
     */
    template<>
    struct JsonArrayParser<bool> : EventHandler
    {
        JsonArrayParser(ReaderContext& context, std::vector<bool>& container)
            : context(context)
            , container(container)
        {}

        bool parse(JsonArrayEnd) override
        {
            context.pop();
            return true;
        }

        bool parse(JsonValueBoolean b) override
        {
            container.push_back(b.value);
            return true;
        }

    private:
        ReaderContext& context;
        std::vector<bool>& container;
    };

    /**
     * Parses a JSON array of numbers into a C++ `vector<T>` efficiently.
     *
     * @tparam Integer or floating-point type.
     */
    template<typename T>
    struct JsonNumberArrayParser : EventHandler
    {
        static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");

        JsonNumberArrayParser(ReaderContext& context, std::vector<T>& container)
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

            container.push_back(value);
            return true;
        }

    private:
        ReaderContext& context;
        std::vector<T>& container;
    };

    template<>
    struct JsonArrayParser<short> : JsonNumberArrayParser<short>
    {
        using JsonNumberArrayParser<short>::JsonNumberArrayParser;
    };

    template<>
    struct JsonArrayParser<unsigned short> : JsonNumberArrayParser<unsigned short>
    {
        using JsonNumberArrayParser<unsigned short>::JsonNumberArrayParser;
    };

    template<>
    struct JsonArrayParser<int> : JsonNumberArrayParser<int>
    {
        using JsonNumberArrayParser<int>::JsonNumberArrayParser;
    };

    template<>
    struct JsonArrayParser<unsigned int> : JsonNumberArrayParser<unsigned int>
    {
        using JsonNumberArrayParser<unsigned int>::JsonNumberArrayParser;
    };

    template<>
    struct JsonArrayParser<long> : JsonNumberArrayParser<long>
    {
        using JsonNumberArrayParser<long>::JsonNumberArrayParser;
    };

    template<>
    struct JsonArrayParser<unsigned long> : JsonNumberArrayParser<unsigned long>
    {
        using JsonNumberArrayParser<unsigned long>::JsonNumberArrayParser;
    };

    template<>
    struct JsonArrayParser<long long> : JsonNumberArrayParser<long long>
    {
        using JsonNumberArrayParser<long long>::JsonNumberArrayParser;
    };

    template<>
    struct JsonArrayParser<unsigned long long> : JsonNumberArrayParser<unsigned long long>
    {
        using JsonNumberArrayParser<unsigned long long>::JsonNumberArrayParser;
    };

    template<>
    struct JsonArrayParser<float> : JsonNumberArrayParser<float>
    {
        using JsonNumberArrayParser<float>::JsonNumberArrayParser;
    };

    template<>
    struct JsonArrayParser<double> : JsonNumberArrayParser<double>
    {
        using JsonNumberArrayParser<double>::JsonNumberArrayParser;
    };

    template<>
    struct JsonArrayParser<std::string> : EventHandler
    {
        JsonArrayParser(ReaderContext& context, std::vector<std::string>& container)
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
            container.emplace_back(s.literal.begin(), s.literal.end());
            return true;
        }

    private:
        ReaderContext& context;
        std::vector<std::string>& container;
    };

    template<typename T>
    struct JsonParser<std::vector<T>> : EventHandler
    {
        using json_type = JsonArrayStart;

        JsonParser(ReaderContext& context, std::vector<T>& ref)
            : context(context)
            , ref(ref)
        {}

        bool parse(JsonArrayStart) override
        {
            context.replace(std::make_unique<JsonArrayParser<T>>(context, ref));
            return true;
        }

    private:
        ReaderContext& context;
        std::vector<T>& ref;
    };
}
