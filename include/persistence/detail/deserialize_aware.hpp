#pragma once
#include "deserialize_context.hpp"

namespace persistence
{
    struct DeserializerContext;

    struct JsonContextAwareDeserializer
    {
        DeserializerContext& context;

        explicit JsonContextAwareDeserializer(DeserializerContext& context) : context(context) {}
    };
}
