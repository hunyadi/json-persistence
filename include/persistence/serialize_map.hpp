#pragma once
#include "serialize_allocate.hpp"
#include "serialize_base.hpp"
#include <map>
#include <unordered_map>

namespace persistence
{
    template<typename C>
    struct JsonDictionarySerializer : JsonAllocatingSerializer
    {
        using JsonAllocatingSerializer::JsonAllocatingSerializer;

        bool operator()(const C& container, rapidjson::Value& json) const
        {
            json.SetObject();
            for (const auto& [key, value] : container) {
                rapidjson::Value json_item;
                if (!serialize<typename C::mapped_type>(value, json_item, allocator)) {
                    return false;
                }
                rapidjson::Value json_key;
                json_key.SetString(key.data(), key.size(), allocator);
                json.AddMember(json_key, json_item, allocator);  // ownership of key and value is transferred
            }
            return true;
        }
    };

    template<typename T>
    struct JsonSerializer<std::map<std::string, T>> : JsonDictionarySerializer<std::map<std::string, T>>
    {
        using JsonDictionarySerializer<std::map<std::string, T>>::JsonDictionarySerializer;
    };

    template<typename T>
    struct JsonSerializer<std::unordered_map<std::string, T>> : JsonDictionarySerializer<std::unordered_map<std::string, T>>
    {
        using JsonDictionarySerializer<std::unordered_map<std::string, T>>::JsonDictionarySerializer;
    };
}
