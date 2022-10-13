#pragma once
#include "detail/engine.hpp"
#include "serialize_allocate.hpp"
#include "serialize_base.hpp"

namespace persistence
{
    template<typename P>
    struct JsonPointerSerializer : JsonAllocatingSerializer
    {
        using JsonAllocatingSerializer::JsonAllocatingSerializer;

        bool operator()(const P& pointer, rapidjson::Value& json) const
        {
            return serialize(*pointer, json, allocator);
        }
    };

    template<typename T>
    struct JsonSerializer<std::unique_ptr<T>> : JsonPointerSerializer<std::unique_ptr<T>>
    {
        using JsonPointerSerializer<std::unique_ptr<T>>::JsonPointerSerializer;
    };

    template<typename T>
    struct JsonSerializer<std::shared_ptr<T>> : JsonPointerSerializer<std::shared_ptr<T>>
    {
        using JsonPointerSerializer<std::shared_ptr<T>>::JsonPointerSerializer;
    };
}
