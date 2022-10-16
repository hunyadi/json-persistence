#pragma once
#include "write_base.hpp"
#include "detail/write_aware.hpp"
#include <tuple>
#include <utility>

namespace persistence
{
    template<typename T1, typename T2>
    struct JsonWriter<std::pair<T1, T2>> : JsonContextAwareWriter
    {
        using JsonContextAwareWriter::JsonContextAwareWriter;

        bool operator()(const std::pair<T1, T2>& container, StringWriter& writer) const
        {
            writer.StartArray();
            WriterContext context_first(context, Segment(0));
            if (!serialize(container.first, writer, context_first)) {
                return false;
            }
            WriterContext context_second(context, Segment(1));
            if (!serialize(container.second, writer, context_second)) {
                return false;
            }
            writer.EndArray();
            return true;
        }
    };

    template<typename... Ts>
    struct JsonWriter<std::tuple<Ts...>> : JsonContextAwareWriter
    {
        using JsonContextAwareWriter::JsonContextAwareWriter;

        bool operator()(const std::tuple<Ts...>& container, StringWriter& writer) const
        {
            writer.StartArray();
            if (!serialize_elements(container, writer, std::make_index_sequence<sizeof...(Ts)>())) {
                return false;
            }
            writer.EndArray();
            return true;
        }

    private:
        template<std::size_t... I>
        bool serialize_elements(const std::tuple<Ts...>& container, StringWriter& writer, std::index_sequence<I...>) const
        {
            return (serialize_element(
                I,
                std::get<I>(container),
                writer
            ) && ...);
        }

        template<typename T>
        bool serialize_element(std::size_t index, const T& item, StringWriter& writer) const
        {
            WriterContext item_context(context, Segment(index));
            if (!serialize(item, writer, item_context)) {
                return false;
            }
            return true;
        }
    };
}
