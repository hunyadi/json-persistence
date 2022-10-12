#pragma once
#include "serialize_allocate.hpp"
#include "serialize_base.hpp"
#include <array>

namespace persistence
{
    template<typename T, std::size_t N>
    struct JsonSerializer<std::array<T, N>> : JsonAllocatingSerializer
    {
        using JsonAllocatingSerializer::JsonAllocatingSerializer;

        bool operator()(const std::array<T, N>& container, rapidjson::Value& json) const
        {
            json.SetArray();
            json.Reserve(container.size(), allocator);
            for (const auto& item : container) {
                rapidjson::Value json_item;
                if (!serialize<T>(item, json_item, allocator)) {
                    return false;
                }
                json.PushBack(json_item, allocator);  // ownership of value is transferred
            }
            return true;
        }
    };
}
