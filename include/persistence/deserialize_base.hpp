#pragma once
#include "detail/config.hpp"
#include <rapidjson/document.h>

namespace persistence
{
    /**
    * Reads a value with a specific type from JSON.
    */
    template<typename T, typename Enable = void>
    struct JsonDeserializer;

    struct DeserializerContext;

    template<typename T>
    bool deserialize(const rapidjson::Value& json, T& obj, DeserializerContext& context);
}
