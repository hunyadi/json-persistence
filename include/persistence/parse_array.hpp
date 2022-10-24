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

        bool parse(typename element_parser_type::json_type json_item) override
        {
            auto element_parser = std::make_unique<element_parser_type>(context, std::get<I - 1>(container));
            auto next_parser = std::make_unique<next_parser_type>(context, container);
            return stateless_parse(context, json_item, std::move(element_parser), std::move(next_parser));
        }

    private:
        bool stateless_parse(
                ReaderContext& ctx,
                typename element_parser_type::json_type json_item,
                std::unique_ptr<element_parser_type>&& element_parser,
                std::unique_ptr<next_parser_type>&& next_parser)
        {
            ctx.replace(std::move(next_parser));
            auto&& handler = ctx.push(std::move(element_parser));
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

        bool parse(JsonArrayStart) override
        {
            context.replace(std::make_unique<JsonFixedArrayParser<C, 1, N - 1>>(context, container));
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

        bool parse(JsonArrayEnd) override
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
