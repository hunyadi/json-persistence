#pragma once
#include "serialize_context.hpp"

namespace persistence
{
    struct JsonContextAwareSerializer
    {
        explicit JsonContextAwareSerializer(SerializerContext& context) : context(context) {}

    protected:
        SerializerContext& context;
    };
}
