#pragma once
#include "output_context.hpp"

namespace persistence
{
    /** Context for serializing an object to a JSON string. */
    struct GlobalWriterContext : WriterReferenceContext
    {};

    using WriterContext = OutputContext<GlobalWriterContext>;
}
