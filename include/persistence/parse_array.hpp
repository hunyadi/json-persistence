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
    struct JsonFixedArrayParser : EventHandler
    {
        using element_type = typename std::tuple_element<I - 1, C>::type;
        using element_parser_type = JsonParser<element_type>;
        using next_parser_type = JsonFixedArrayParser<C, I + 1, N - 1>;

        JsonFixedArrayParser(ReaderContext& context, C& container)
            : context(context)
            , container(container)
        {}

        bool parse(const typename element_parser_type::json_type& json_item) override
        {
            return stateless_parse(context, container, json_item);
        }

    private:
        static bool stateless_parse(ReaderContext& context, C& container, const typename element_parser_type::json_type& json_item)
        {
            context.replace<next_parser_type>(context, container);
            auto&& handler = context.emplace<element_parser_type>(context, std::get<I - 1>(container));
            return handler.parse(json_item);
        }

    private:
        ReaderContext& context;
        C& container;
    };

    /** Parses the start of a tuple-like type. */
    template<typename C, std::size_t N>
    struct JsonFixedArrayParser<C, 0, N> : EventHandler
    {
        JsonFixedArrayParser(ReaderContext& context, C& container)
            : context(context)
            , container(container)
        {}

        bool parse(const JsonArrayStart&) override
        {
            context.replace<JsonFixedArrayParser<C, 1, N - 1>>(context, container);
            return true;
        }

    private:
        ReaderContext& context;
        C& container;
    };

    /** Parses the end of a tuple-like type. */
    template<typename C, std::size_t I>
    struct JsonFixedArrayParser<C, I, 0> : EventHandler
    {
        JsonFixedArrayParser(ReaderContext& context, C& container)
            : context(context)
            , container(container)
        {}

        bool parse(const JsonArrayEnd&) override
        {
            context.pop();
            return true;
        }

    private:
        ReaderContext& context;
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
