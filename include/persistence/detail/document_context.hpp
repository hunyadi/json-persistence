#pragma once
#include "config.hpp"
#include <rapidjson/document.h>

namespace persistence
{
    struct DocumentContext
    {
    public:
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
