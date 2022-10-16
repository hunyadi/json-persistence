#pragma once
#include "write_context.hpp"

namespace persistence
{
    struct JsonContextAwareWriter
    {
        WriterContext& context;

        explicit JsonContextAwareWriter(WriterContext& context) : context(context) {}
    };
}
