#pragma once
#include "engine.hpp"

namespace persistence
{
    struct GlobalContext
    {
    public:
        GlobalContext(rapidjson::Document& document)
            : document(document)
        {}

        rapidjson::MemoryPoolAllocator<>& allocator()
        {
            return document.GetAllocator();
        }

        rapidjson::Document& document;
    };
}
