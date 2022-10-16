#pragma once
#include "deserialize_context.hpp"

namespace persistence
{
    struct JsonContextAwareDeserializer
    {
        DeserializerContext& context;

        explicit JsonContextAwareDeserializer(DeserializerContext& context) : context(context) {}
    };
}
