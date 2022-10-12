#pragma once
#include "serialize_allocate.hpp"
#include "serialize_base.hpp"
#include <vector>

namespace persistence
{
    template<typename T>
    struct JsonSerializer<std::vector<T>> : JsonAllocatingSerializer
    {
        using JsonAllocatingSerializer::JsonAllocatingSerializer;

        bool operator()(const std::vector<T>& container, rapidjson::Value& json) const
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
