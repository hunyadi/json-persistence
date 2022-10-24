#pragma once
#include "detail/defer.hpp"
#include "detail/serialize_context.hpp"

namespace persistence
{
    /**
    * Writes a value with a specific type to a JSON DOM.
    */
    template<typename T, typename Enable = void>
    struct JsonSerializer
    {
        // if you are getting a compile-time error pointing at this location, make sure the appropriate
        // headers are included, and (de-)serialization is supported for the type
        static_assert(detail::defer<T>::value, "expected a type that can be serialized to JSON");
    };

    template<typename T>
    bool serialize(const T& obj, rapidjson::Value& json, SerializerContext& context);
}
