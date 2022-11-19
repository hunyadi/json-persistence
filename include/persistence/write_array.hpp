#pragma once
#include "write_base.hpp"
#include "detail/write_aware.hpp"
#include "detail/unlikely.hpp"
#include <array>

namespace persistence
{
    template<typename T, std::size_t N>
    struct JsonWriter<std::array<T, N>> : JsonContextAwareWriter
    {
        using JsonContextAwareWriter::JsonContextAwareWriter;

        bool operator()(const std::array<T, N>& container, StringWriter& writer) const
        {
            writer.StartArray();
            std::size_t idx = 0;
            for (const auto& item : container) {
                WriterContext item_context(context, Segment(idx));
                PERSISTENCE_IF_UNLIKELY(!serialize<T>(item, writer, item_context)) {
                    return false;
                }
                ++idx;
            }
            writer.EndArray();
            return true;
        }
    };
}
