#pragma once
#include "write_context.hpp"

namespace persistence
{
    struct JsonContextAwareWriter
    {
        explicit JsonContextAwareWriter(WriterContext& context) : context(context) {}

    protected:
        WriterContext& context;
    };
}
