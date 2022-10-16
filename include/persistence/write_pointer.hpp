#pragma once
#include "write_base.hpp"
#include "detail/write_aware.hpp"
#include "detail/path.hpp"

namespace persistence
{
    template<typename T>
    struct JsonWriter<std::unique_ptr<T>> : JsonContextAwareWriter
    {
        using JsonContextAwareWriter::JsonContextAwareWriter;

        bool operator()(const std::unique_ptr<T>& pointer, StringWriter& writer) const
        {
            return serialize(*pointer, writer, context);
        }
    };

    template<typename T>
    struct JsonWriter<std::shared_ptr<T>> : JsonContextAwareWriter
    {
        using JsonContextAwareWriter::JsonContextAwareWriter;

        bool operator()(const std::shared_ptr<T>& pointer, StringWriter& writer) const
        {
            auto&& segments = context.get(pointer);
            if (!segments.empty()) {
                std::string ref = Path(segments).str();

                // create a {"$ref": "/path/to/earlier/occurrence"}
                writer.StartObject();
                writer.Key("$ref");
                writer.String(ref.data(), ref.size(), true);
                writer.EndObject();

            } else {
                if (!serialize(*pointer, writer, context)) {
                    return false;
                }

                // assign a "/path/to/this/occurrence"
                context.put(pointer);
            }
            return true;
        }
    };
}
