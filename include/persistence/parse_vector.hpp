#pragma once
#include "parse_base.hpp"
#include "parse_items.hpp"
#include "parse_fundamental.hpp"
#include "detail/unlikely.hpp"
#include <vector>

namespace persistence
{
    /**
     * Parses a JSON array of possibly composite values into a C++ `vector<T>`.
     */
    template<typename T>
    struct JsonArrayParser : JsonArrayItemParseHandler<T>
    {
        JsonArrayParser(ReaderContext& context, std::vector<T>& container)
            : JsonArrayItemParseHandler<T>(context)
            , container(container)
        {}

        bool parse(const JsonArrayEnd&) override
        {
            this->context.pop();
            return true;
        }

        bool parse(const typename JsonParser<T>::json_type& json_item) override
        {
            container.emplace_back();
            ReaderContext& ctx = this->context;
            auto&& handler = ctx.emplace<JsonParser<T>>(ctx, container.back());
            return handler.parse(json_item);
        }

    private:
        std::vector<T>& container;
    };

    /**
     * Parses a JSON array of boolean values into a C++ `vector<bool>` efficiently.
     */
    template<>
    struct JsonArrayParser<bool> : JsonParseHandler<JsonValueBoolean, JsonArrayEnd>
    {
        JsonArrayParser(ReaderContext& context, std::vector<bool>& container)
            : JsonParseHandler(context)
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
    struct JsonNumberArrayParser : JsonParseHandler<JsonValueNumber, JsonArrayEnd>
    {
        static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type");

        JsonNumberArrayParser(ReaderContext& context, std::vector<T>& container)
            : JsonParseHandler(context)
            , container(container)
        {}

        bool parse(const JsonArrayEnd&) override
        {
            context.pop();
            return true;
        }

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
            T value;
            PERSISTENCE_IF_UNLIKELY(!JsonNumberValueParser<T>::parse(context, n.literal, value)) {
                return false;
            }

            container.push_back(value);
            return true;
        }

    private:
        template<typename V>
        bool parse_number(V value)
        {
            T item_value;
            PERSISTENCE_IF_UNLIKELY(!JsonNumberValueParser<T>::parse(context, value, item_value)) {
                return false;
            }

            container.push_back(item_value);
            return true;
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
    struct JsonArrayParser<std::string> : JsonParseHandler<JsonValueString, JsonArrayEnd>
    {
        JsonArrayParser(ReaderContext& context, std::vector<std::string>& container)
            : JsonParseHandler(context)
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
    struct JsonParser<std::vector<T>> : JsonParseHandler<JsonArrayStart>
    {
        using json_type = JsonArrayStart;

        JsonParser(ReaderContext& context, std::vector<T>& ref)
            : JsonParseHandler<JsonArrayStart>(context)
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
