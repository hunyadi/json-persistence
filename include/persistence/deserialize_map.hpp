#pragma once
#include "deserialize_base.hpp"
#include "deserialize_check.hpp"
#include "detail/deserialize_aware.hpp"
#include <map>
#include <unordered_map>

namespace persistence
{
    template<bool Exception, typename C>
    struct JsonDictionaryDeserializer : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, C& value) const
        {
            using item_type = typename C::mapped_type;

            if (!detail::check_object<Exception>(json, context)) {
                return false;
            }

            value.clear();
            for (auto&& it = json.MemberBegin(); it != json.MemberEnd(); ++it) {
                item_type item;
                DeserializerContext item_context(context, Segment(it->name.GetString()));
                if (!deserialize<Exception>(it->value, item, item_context)) {
                    return false;
                }
                value[it->name.GetString()] = std::move(item);
            }
            return true;
        }
    };

    template<bool Exception, typename T>
    struct JsonDeserializer<Exception, std::map<std::string, T>> : JsonDictionaryDeserializer<Exception, std::map<std::string, T>>
    {
        using JsonDictionaryDeserializer<Exception, std::map<std::string, T>>::JsonDictionaryDeserializer;
    };

    template<bool Exception, typename T>
    struct JsonDeserializer<Exception, std::unordered_map<std::string, T>> : JsonDictionaryDeserializer<Exception, std::unordered_map<std::string, T>>
    {
        using JsonDictionaryDeserializer<Exception, std::unordered_map<std::string, T>>::JsonDictionaryDeserializer;
    };
}
