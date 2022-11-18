#pragma once
#include "output_context.hpp"

namespace persistence
{
    /** Context for serializing an object to a JSON string. */
    using WriterContext = OutputContext<WriterReferenceContext>;
}
