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
    struct JsonFixedArrayParser : JsonSingleEventHandler<typename JsonParser<std::tuple_element_t<I - 1, C>>::json_type>
    {
        using element_type = std::tuple_element_t<I - 1, C>;
        using element_parser_type = JsonParser<element_type>;
        using element_json_type = typename element_parser_type::json_type;
        using next_parser_type = JsonFixedArrayParser<C, I + 1, N - 1>;

        JsonFixedArrayParser(ReaderContext& context, C& container)
            : JsonSingleEventHandler<element_json_type>(context)
            , container(container)
        {}

        bool parse(const element_json_type& json_item) override
        {
            return stateless_parse(this->context, container, json_item);
        }

    private:
        static bool stateless_parse(ReaderContext& context, C& container, const typename element_parser_type::json_type& json_item)
        {
            context.replace<next_parser_type>(context, container);
            auto&& handler = context.emplace<element_parser_type>(context, std::get<I - 1>(container));
            return handler.parse(json_item);
        }

    private:
        C& container;
    };

    /** Parses the start of a tuple-like type. */
    template<typename C, std::size_t N>
    struct JsonFixedArrayParser<C, 0, N> : JsonSingleEventHandler<JsonArrayStart>
    {
        JsonFixedArrayParser(ReaderContext& context, C& container)
            : JsonSingleEventHandler<JsonArrayStart>(context)
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
    struct JsonFixedArrayParser<C, I, 0> : JsonSingleEventHandler<JsonArrayEnd>
    {
        JsonFixedArrayParser(ReaderContext& context, C& container)
            : JsonSingleEventHandler<JsonArrayEnd>(context)
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
