#pragma once
#include "deserialize_base.hpp"
#include "detail/deserialize_aware.hpp"
#include <map>
#include <unordered_map>

namespace persistence
{
    template<typename C>
    struct JsonDictionaryDeserializer : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, C& value) const
        {
            using item_type = typename C::mapped_type;

            if (!json.IsObject()) {
                return false;
            }

            value.clear();
            for (auto&& it = json.MemberBegin(); it != json.MemberEnd(); ++it) {
                item_type item;
                DeserializerContext item_context(context, Segment(it->name.GetString()));
                if (!deserialize(it->value, item, item_context)) {
                    return false;
                }
                if (!it->name.IsString()) {
                    return false;
                }
                value[it->name.GetString()] = std::move(item);
            }
            return true;
        }
    };

    template<typename T>
    struct JsonDeserializer<std::map<std::string, T>> : JsonDictionaryDeserializer<std::map<std::string, T>>
    {
        using JsonDictionaryDeserializer<std::map<std::string, T>>::JsonDictionaryDeserializer;
    };

    template<typename T>
    struct JsonDeserializer<std::unordered_map<std::string, T>> : JsonDictionaryDeserializer<std::unordered_map<std::string, T>>
    {
        using JsonDictionaryDeserializer<std::unordered_map<std::string, T>>::JsonDictionaryDeserializer;
    };
}
