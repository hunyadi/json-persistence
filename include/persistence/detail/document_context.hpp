#pragma once
#include "config.hpp"
#include <rapidjson/document.h>

namespace persistence
{
    /** A context that provides access to the JSON DOM document. */
    struct DocumentContext
    {
        DocumentContext(rapidjson::Document& document)
            : document(document)
        {}

        rapidjson::MemoryPoolAllocator<>& allocator()
        {
            return document.GetAllocator();
        }

        rapidjson::Document& document;
    };
}
