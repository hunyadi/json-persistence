#pragma once
#include "parse_base.hpp"
#include "parse_fundamental.hpp"
#include <set>
#include <vector>

namespace persistence
{
    /**
     * Parses a JSON array of possibly composite values into a C++ `set<T>`.
     */
    template<typename T>
    struct JsonSetParser : JsonEventHandler
    {
        JsonSetParser(ReaderContext& context, std::set<T>& container)
            : JsonEventHandler(context)
            , container(container)
        {}

        bool parse(const JsonArrayEnd&) override
        {
            container.insert(storage.begin(), storage.end());
            context.pop();
            return true;
        }

        bool parse(const typename JsonParser<T>::json_type& json_item) override
        {
            storage.emplace_back();
            auto&& handler = context.emplace<JsonParser<T>>(context, storage.back());
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
    struct JsonSetParser<bool> : JsonEventHandler
    {
        JsonSetParser(ReaderContext& context, std::set<bool>& container)
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
    struct JsonNumberSetParser : JsonEventHandler
    {
        static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type");

        JsonNumberSetParser(ReaderContext& context, std::set<T>& container)
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
                container.insert(value);
                return true;
            } else {
                return false;
            }
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
    struct JsonSetParser<std::string> : JsonEventHandler
    {
        JsonSetParser(ReaderContext& context, std::set<std::string>& container)
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
            container.emplace(s.literal.begin(), s.literal.end());
            return true;
        }

    private:
        std::set<std::string>& container;
    };

    template<typename T>
    struct JsonParser<std::set<T>> : JsonSingleEventHandler<JsonArrayStart>
    {
        using json_type = JsonArrayStart;

        JsonParser(ReaderContext& context, std::set<T>& ref)
            : JsonSingleEventHandler<JsonArrayStart>(context)
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
