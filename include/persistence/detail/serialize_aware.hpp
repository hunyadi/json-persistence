#pragma once
#include "serialize_context.hpp"

namespace persistence
{
    struct JsonContextAwareSerializer
    {
        SerializerContext& context;

        explicit JsonContextAwareSerializer(SerializerContext& context) : context(context) {}
    };
}
