#pragma once
#include "detail/version.hpp"
#include "detail/defer.hpp"
#include "detail/write_context.hpp"
#include "detail/config.hpp"
#include <rapidjson/writer.h>

namespace persistence
{
    /**
    * Writes a value with a specific type to a JSON string.
    */
    template<typename T, typename Enable = void>
    struct JsonWriter
    {
        // if you are getting a compile-time error pointing at this location, make sure the appropriate
        // headers are included, and (de-)serialization is supported for the type
        static_assert(detail::fail<T>, "expected a type that can be serialized to JSON");
    };

    using StringWriter = rapidjson::Writer<rapidjson::StringBuffer>;

    template<typename T>
    bool serialize(const T& obj, StringWriter& writer, WriterContext& context);
}
