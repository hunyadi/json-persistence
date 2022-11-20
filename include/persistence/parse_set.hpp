#pragma once
#include "parse_base.hpp"
#include "parse_items.hpp"
#include "parse_fundamental.hpp"
#include "detail/unlikely.hpp"
#include <set>
#include <vector>

namespace persistence
{
    /**
     * Parses a JSON array of possibly composite values into a C++ `set<T>`.
     */
    template<typename T>
    struct JsonSetParser : JsonArrayItemParseHandler<T>
    {
        JsonSetParser(ReaderContext& context, std::set<T>& container)
            : JsonArrayItemParseHandler<T>(context)
            , container(container)
        {}

        bool parse(const JsonArrayEnd&) override
        {
            for (auto&& item : storage) {
                container.insert(std::move(item));
            }
            storage.clear();
            this->context.pop();
            return true;
        }

        bool parse(const typename JsonParser<T>::json_type& json_item) override
        {
            storage.emplace_back();
            ReaderContext& ctx = this->context;
            auto&& handler = ctx.emplace<JsonParser<T>>(ctx, storage.back());
            return handler.parse(json_item);
        }

    private:
        std::vector<T> storage;
        std::set<T>& container;
    };

    /**
     * Parses a JSON array of boolean values into a C++ `set<bool>` efficiently.
     */
    template<>
    struct JsonSetParser<bool> : JsonParseHandler<JsonValueBoolean, JsonArrayEnd>
    {
        JsonSetParser(ReaderContext& context, std::set<bool>& container)
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
            container.insert(b.value);
            return true;
        }

    private:
        std::set<bool>& container;
    };

    /**
     * Parses a JSON array of numbers into a C++ `set<T>` efficiently.
     *
     * @tparam Integer or floating-point type.
     */
    template<typename T>
    struct JsonNumberSetParser : JsonParseHandler<JsonValueNumber, JsonArrayEnd>
    {
        static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type");

        JsonNumberSetParser(ReaderContext& context, std::set<T>& container)
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

            container.insert(value);
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

            container.insert(item_value);
            return true;
        }

    private:
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
    struct JsonSetParser<std::string> : JsonParseHandler<JsonValueString, JsonArrayEnd>
    {
        JsonSetParser(ReaderContext& context, std::set<std::string>& container)
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
            container.emplace(s.literal.begin(), s.literal.end());
            return true;
        }

    private:
        std::set<std::string>& container;
    };

    template<typename T>
    struct JsonParser<std::set<T>> : JsonParseHandler<JsonArrayStart>
    {
        using json_type = JsonArrayStart;

        JsonParser(ReaderContext& context, std::set<T>& ref)
            : JsonParseHandler(context)
            , ref(ref)
        {}

        bool parse(const JsonArrayStart&) override
        {
            context.replace<JsonSetParser<T>>(context, ref);
            return true;
        }

    private:
        std::set<T>& ref;
    };
}
