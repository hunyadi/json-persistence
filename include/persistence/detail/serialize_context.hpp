#pragma once
#include "document_context.hpp"
#include "output_context.hpp"

namespace persistence
{
    /** Context for serializing an object to a JSON DOM document. */
    struct GlobalSerializerContext : DocumentContext, WriterReferenceContext
    {
        using DocumentContext::DocumentContext;
    };

    using SerializerContext = OutputContext<GlobalSerializerContext>;
}
