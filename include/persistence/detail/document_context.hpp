#pragma once
#include "config.hpp"
#include <rapidjson/document.h>

namespace persistence
{
    /** A context that provides access to the JSON DOM document. */
    struct DocumentContext
    {
        DocumentContext(rapidjson::Document& document)
            : doc(document)
        {}

        rapidjson::Document& document()
        {
            return doc;
        }

        rapidjson::MemoryPoolAllocator<>& allocator()
        {
            return doc.GetAllocator();
        }

    private:
        rapidjson::Document& doc;
    };
}
