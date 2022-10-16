#pragma once
#include "detail/write_context.hpp"
#include "detail/config.hpp"
#include <rapidjson/writer.h>

namespace persistence
{
    /**
    * Writes a value with a specific type to a JSON string.
    */
    template<typename T, typename Enable = void>
    struct JsonWriter;

    using StringWriter = rapidjson::Writer<rapidjson::StringBuffer>;

    template<typename T>
    bool serialize(const T& obj, StringWriter& writer, WriterContext& context);
}
