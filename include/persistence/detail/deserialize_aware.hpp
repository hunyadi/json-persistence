#pragma once
#include "deserialize_context.hpp"

namespace persistence
{
    struct JsonContextAwareDeserializer
    {
        explicit JsonContextAwareDeserializer(DeserializerContext& context) : context(context) {}

    protected:
        DeserializerContext& context;
    };
}
