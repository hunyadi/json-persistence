#pragma once
#include "detail/engine.hpp"

namespace persistence
{
    struct JsonAllocatingSerializer
    {
        rapidjson::MemoryPoolAllocator<>& allocator;

        explicit JsonAllocatingSerializer(rapidjson::MemoryPoolAllocator<>& allocator) : allocator(allocator) {}
    };
}
