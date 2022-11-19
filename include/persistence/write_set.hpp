#pragma once
#include "write_base.hpp"
#include "detail/write_aware.hpp"
#include "detail/unlikely.hpp"
#include <set>

namespace persistence
{
    template<typename T>
    struct JsonWriter<std::set<T>> : JsonContextAwareWriter
    {
        using JsonContextAwareWriter::JsonContextAwareWriter;

        bool operator()(const std::set<T>& container, StringWriter& writer) const
        {
            writer.StartArray();
            std::size_t k = 0;
            for (const auto& item : container) {
                WriterContext item_context(context, Segment(k));
                PERSISTENCE_IF_UNLIKELY(!serialize<T>(item, writer, context)) {
                    return false;
                }
                ++k;
            }
            writer.EndArray();
            return true;
        }
    };
}
