#pragma once
#include "detail/engine.hpp"

namespace persistence
{
    /**
    * Writes a value with a specific type to JSON.
    */
    template<typename T, typename Enable = void>
    struct JsonSerializer;

    struct SerializerContext;

    template<typename T>
    bool serialize(const T& obj, rapidjson::Value& json, SerializerContext& context);
}
