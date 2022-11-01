#pragma once
#include "detail/version.hpp"
#include "detail/config.hpp"
#include "detail/defer.hpp"
#include <rapidjson/document.h>

namespace persistence
{
    /**
     * Reads a value with a specific type from JSON DOM.
     */
    template<bool Exception, typename T, typename Enable = void>
    struct JsonDeserializer
    {
        // if you are getting a compile-time error pointing at this location, make sure the appropriate
        // headers are included, and (de-)serialization is supported for the type
        static_assert(detail::defer<T>::value, "expected a type that can be deserialized from JSON");
    };

    struct DeserializerContext;

    /**
     * Deserializes a C++ object from a JSON DOM representation.
     *
     * @tparam Exception True if exceptions may be thrown to signal deserialization failures.
     * @tparam T Type to be deserialized.
     */
    template<bool Exception, typename T>
    bool deserialize(const rapidjson::Value& json, T& obj, DeserializerContext& context);
}
