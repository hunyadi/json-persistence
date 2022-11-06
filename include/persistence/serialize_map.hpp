#pragma once
#include "serialize_base.hpp"
#include "detail/serialize_aware.hpp"
#include <map>
#include <unordered_map>

namespace persistence
{
    template<typename C>
    struct JsonDictionarySerializer : JsonContextAwareSerializer
    {
        using JsonContextAwareSerializer::JsonContextAwareSerializer;

        bool operator()(const C& container, rapidjson::Value& json) const
        {
            json.SetObject();
            for (auto&& [key, value] : container) {
                rapidjson::Value value_json;
                SerializerContext value_context(context, Segment(key));
                if (!serialize<typename C::mapped_type>(value, value_json, value_context)) {
                    return false;
                }

                rapidjson::Value key_json;
                key_json.SetString(key.data(), static_cast<rapidjson::SizeType>(key.size()), context.global().allocator());
                json.AddMember(key_json, value_json, context.global().allocator());  // ownership of key and value is transferred
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
