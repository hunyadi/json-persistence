#pragma once
#include "write_base.hpp"
#include "detail/write_aware.hpp"
#include <tuple>
#include <utility>

namespace persistence
{
    template<typename C, std::size_t S>
    struct JsonTupleWriter : JsonContextAwareWriter
    {
        using JsonContextAwareWriter::JsonContextAwareWriter;

        bool operator()(const C& container, StringWriter& writer) const
        {
            writer.StartArray();
            if (!serialize_elements(container, writer, std::make_index_sequence<S>())) {
                return false;
            }
            writer.EndArray();
            return true;
        }

    private:
        template<std::size_t... I>
        bool serialize_elements(const C& container, StringWriter& writer, std::index_sequence<I...>) const
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

    template<typename T1, typename T2>
    struct JsonWriter<std::pair<T1, T2>> : JsonTupleWriter<std::pair<T1, T2>, 2>
    {
        using JsonTupleWriter<std::pair<T1, T2>, 2>::JsonTupleWriter;
    };

    template<typename... Ts>
    struct JsonWriter<std::tuple<Ts...>> : JsonTupleWriter<std::tuple<Ts...>, sizeof...(Ts)>
    {
        using JsonTupleWriter<std::tuple<Ts...>, sizeof...(Ts)>::JsonTupleWriter;
    };
}
