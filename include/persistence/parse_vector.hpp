#pragma once
#include "parse_base.hpp"
#include "parse_fundamental.hpp"
#include "detail/unlikely.hpp"
#include <vector>

namespace persistence
{
    /**
     * Parses a JSON array of possibly composite values into a C++ `vector<T>`.
     */
    template<typename T>
    struct JsonArrayParser : JsonEventHandler
    {
        JsonArrayParser(ReaderContext& context, std::vector<T>& container)
            : JsonEventHandler(context)
            , container(container)
        {}

        bool parse(const JsonArrayEnd&) override
        {
            context.pop();
            return true;
        }

        bool parse(const typename JsonParser<T>::json_type& json_item) override
        {
            container.emplace_back();
            auto&& handler = context.emplace<JsonParser<T>>(context, container.back());
            return handler.parse(json_item);
        }

    private:
        std::vector<T>& container;
    };

    /**
     * Parses a JSON array of boolean values into a C++ `vector<bool>` efficiently.
     */
    template<>
    struct JsonArrayParser<bool> : JsonEventHandler
    {
        JsonArrayParser(ReaderContext& context, std::vector<bool>& container)
            : JsonEventHandler(context)
            , container(container)
        {}

        bool parse(const JsonArrayEnd&) override
        {
            context.pop();
            return true;
        }

        bool parse(const JsonValueBoolean& b) override
        {
            container.push_back(b.value);
            return true;
        }

    private:
        std::vector<bool>& container;
    };

    /**
     * Parses a JSON array of numbers into a C++ `vector<T>` efficiently.
     *
     * @tparam Integer or floating-point type.
     */
    template<typename T>
    struct JsonNumberArrayParser : JsonEventHandler
    {
        static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type");

        JsonNumberArrayParser(ReaderContext& context, std::vector<T>& container)
            : JsonEventHandler(context)
            , container(container)
        {}

        bool parse(const JsonArrayEnd&) override
        {
            context.pop();
            return true;
        }

        bool parse(const JsonValueNumber& n) override
        {
            T value;
            if (JsonNumberValueParser<T>::parse(context, n, value)) {
                container.push_back(value);
                return true;
            } else {
                return false;
            }
        }

    private:
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
    struct JsonArrayParser<std::string> : JsonEventHandler
    {
        JsonArrayParser(ReaderContext& context, std::vector<std::string>& container)
            : JsonEventHandler(context)
            , container(container)
        {}

        bool parse(const JsonArrayEnd&) override
        {
            context.pop();
            return true;
        }

        bool parse(const JsonValueString& s) override
        {
            container.emplace_back(s.literal.begin(), s.literal.end());
            return true;
        }

    private:
        std::vector<std::string>& container;
    };

    template<typename T>
    struct JsonParser<std::vector<T>> : JsonSingleEventHandler<JsonArrayStart>
    {
        using json_type = JsonArrayStart;

        JsonParser(ReaderContext& context, std::vector<T>& ref)
            : JsonSingleEventHandler<JsonArrayStart>(context)
            , ref(ref)
        {}

        bool parse(const JsonArrayStart&) override
        {
            context.replace<JsonArrayParser<T>>(context, ref);
            return true;
        }

    private:
        std::vector<T>& ref;
    };
}
