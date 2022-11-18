#pragma once
#include "write_base.hpp"
#include "detail/write_aware.hpp"
#include "detail/path.hpp"
#include <memory>

namespace persistence
{
    template<typename P>
    struct JsonPointerWriter : JsonContextAwareWriter
    {
        using JsonContextAwareWriter::JsonContextAwareWriter;

        bool operator()(const P& pointer, StringWriter& writer) const
        {
            return serialize(*pointer, writer, context);
        }
    };

    template<typename T>
    struct JsonWriter<T*> : JsonPointerWriter<T*>
    {
        using JsonPointerWriter<T*>::JsonPointerWriter;
    };

    template<typename T>
    struct JsonWriter<std::unique_ptr<T>> : JsonPointerWriter<std::unique_ptr<T>>
    {
        using JsonPointerWriter<std::unique_ptr<T>>::JsonPointerWriter;
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
                writer.String(ref.data(), static_cast<rapidjson::SizeType>(ref.size()), true);
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
