#pragma once
#include "deserialize_base.hpp"
#include "detail/engine.hpp"
#include <map>
#include <unordered_map>

namespace persistence
{
    template<typename C>
    struct JsonDictionaryDeserializer
    {
        bool operator()(const rapidjson::Value& json, C& value) const
        {
            if (!json.IsObject()) {
                return false;
            }

            value.clear();
            for (auto&& it = json.MemberBegin(); it != json.MemberEnd(); ++it) {
                typename C::mapped_type item;
                if (!deserialize(it->value, item)) {
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
    struct JsonDeserializer<std::map<std::string, T>> : JsonDictionaryDeserializer< std::map<std::string, T>> {};

    template<typename T>
    struct JsonDeserializer<std::unordered_map<std::string, T>> : JsonDictionaryDeserializer< std::unordered_map<std::string, T>> {};
}
