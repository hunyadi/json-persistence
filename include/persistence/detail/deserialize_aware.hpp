#pragma once

namespace persistence
{
    struct DeserializerContext;

    struct JsonContextAwareDeserializer
    {
        explicit JsonContextAwareDeserializer(DeserializerContext& context) : context(context) {}

    protected:
        DeserializerContext& context;
    };
}
