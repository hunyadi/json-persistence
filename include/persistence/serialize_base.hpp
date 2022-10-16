#pragma once
#include "detail/serialize_context.hpp"

namespace persistence
{
    /**
    * Writes a value with a specific type to a JSON DOM.
    */
    template<typename T, typename Enable = void>
    struct JsonSerializer;

    template<typename T>
    bool serialize(const T& obj, rapidjson::Value& json, SerializerContext& context);
}
