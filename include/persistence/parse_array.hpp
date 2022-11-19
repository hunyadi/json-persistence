#pragma once
#include "parse_base.hpp"
#include <array>

namespace persistence
{
    /**
     * Parses the elements of a tuple-like type.
     * @tparam C A tuple-like type such as an `array`, `pair` or `tuple`.
     * @tparam I The index of the current item, including JSON array start/end brackets.
     * @tparam N The remaining number of items, including JSON array start/end brackets.
     */
    template<typename C, std::size_t I, std::size_t N>
    struct JsonFixedArrayParser : JsonParseSingleHandler<typename JsonParser<std::tuple_element_t<I - 1, C>>::json_type>
    {
        using element_type = std::tuple_element_t<I - 1, C>;
        using element_parser_type = JsonParser<element_type>;
        using element_json_type = typename element_parser_type::json_type;
        using next_parser_type = JsonFixedArrayParser<C, I + 1, N - 1>;

        JsonFixedArrayParser(ReaderContext& context, C& container)
            : JsonParseSingleHandler<element_json_type>(context)
            , container(container)
        {}

        bool parse(const JsonValueNull& item) override
        {
            return stateless_parse(this->context, container, item);
        }

        bool parse(const JsonValueBoolean& item) override
        {
            return stateless_parse(this->context, container, item);
        }

        bool parse(const JsonValueInteger& item) override
        {
            return stateless_parse(this->context, container, item);
        }

        bool parse(const JsonValueUnsigned& item) override
        {
            return stateless_parse(this->context, container, item);
        }

        bool parse(const JsonValueInteger64& item) override
        {
            return stateless_parse(this->context, container, item);
        }

        bool parse(const JsonValueUnsigned64& item) override
        {
            return stateless_parse(this->context, container, item);
        }

        bool parse(const JsonValueDouble& item) override
        {
            return stateless_parse(this->context, container, item);
        }

        bool parse(const JsonValueNumber& item) override
        {
            return stateless_parse(this->context, container, item);
        }

        bool parse(const JsonValueString& item) override
        {
            return stateless_parse(this->context, container, item);
        }

        bool parse(const JsonObjectStart& item) override
        {
            return stateless_parse(this->context, container, item);
        }

        bool parse(const JsonArrayStart& item) override
        {
            return stateless_parse(this->context, container, item);
        }

    private:
        template<typename TokenType>
        static bool stateless_parse(ReaderContext& context, C& container, const TokenType& item)
        {
            context.replace<next_parser_type>(context, container);
            auto&& handler = context.emplace<element_parser_type>(context, std::get<I - 1>(container));
            return handler.parse(item);
        }

    private:
        C& container;
    };

    /** Parses the start of a tuple-like type. */
    template<typename C, std::size_t N>
    struct JsonFixedArrayParser<C, 0, N> : JsonParseSingleHandler<JsonArrayStart>
    {
        JsonFixedArrayParser(ReaderContext& context, C& container)
            : JsonParseSingleHandler<JsonArrayStart>(context)
            , container(container)
        {}

        bool parse(const JsonArrayStart&) override
        {
            context.replace<JsonFixedArrayParser<C, 1, N - 1>>(context, container);
            return true;
        }

    private:
        C& container;
    };

    /** Parses the end of a tuple-like type. */
    template<typename C, std::size_t I>
    struct JsonFixedArrayParser<C, I, 0> : JsonParseSingleHandler<JsonArrayEnd>
    {
        JsonFixedArrayParser(ReaderContext& context, C& container)
            : JsonParseSingleHandler<JsonArrayEnd>(context)
            , container(container)
        {}

        bool parse(const JsonArrayEnd&) override
        {
            context.pop();
            return true;
        }

    private:
        C& container;
    };

    template<typename T, std::size_t N>
    struct JsonParser<std::array<T, N>> : JsonFixedArrayParser<std::array<T, N>, 0, N + 1>
    {
        using json_type = JsonArrayStart;

        using JsonFixedArrayParser<std::array<T, N>, 0, N + 1>::JsonFixedArrayParser;
    };

    template<typename T1, typename T2>
    struct JsonParser<std::pair<T1, T2>> : JsonFixedArrayParser<std::pair<T1, T2>, 0, 3>
    {
        using json_type = JsonArrayStart;

        using JsonFixedArrayParser<std::pair<T1, T2>, 0, 3>::JsonFixedArrayParser;
    };

    template<typename... T>
    struct JsonParser<std::tuple<T...>> : JsonFixedArrayParser<std::tuple<T...>, 0, sizeof...(T) + 1>
    {
        using json_type = JsonArrayStart;

        using JsonFixedArrayParser<std::tuple<T...>, 0, sizeof...(T) + 1>::JsonFixedArrayParser;
    };
}
