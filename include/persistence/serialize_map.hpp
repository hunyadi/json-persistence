#pragma once
#include "dictionary.hpp"
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
                if (!serialize<typename C::value_type::second_type>(value, value_json, value_context)) {
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
    struct JsonSerializer<literal_dict<T>> : JsonDictionarySerializer<literal_dict<T>>
    {
        using JsonDictionarySerializer<literal_dict<T>>::JsonDictionarySerializer;
    };

    template<typename T>
    struct JsonSerializer<literal_map<T>> : JsonDictionarySerializer<literal_map<T>>
    {
        using JsonDictionarySerializer<literal_map<T>>::JsonDictionarySerializer;
    };

    template<typename T>
    struct JsonSerializer<literal_unordered_map<T>> : JsonDictionarySerializer<literal_unordered_map<T>>
    {
        using JsonDictionarySerializer<literal_unordered_map<T>>::JsonDictionarySerializer;
    };

    template<typename T>
    struct JsonSerializer<string_dict<T>> : JsonDictionarySerializer<string_dict<T>>
    {
        using JsonDictionarySerializer<string_dict<T>>::JsonDictionarySerializer;
    };

    template<typename T>
    struct JsonSerializer<string_map<T>> : JsonDictionarySerializer<string_map<T>>
    {
        using JsonDictionarySerializer<string_map<T>>::JsonDictionarySerializer;
    };

    template<typename T>
    struct JsonSerializer<string_unordered_map<T>> : JsonDictionarySerializer<string_unordered_map<T>>
    {
        using JsonDictionarySerializer<string_unordered_map<T>>::JsonDictionarySerializer;
    };
}
