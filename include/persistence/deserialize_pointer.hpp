#pragma once
#include "deserialize_base.hpp"

namespace persistence
{
    template<typename T>
    struct JsonDeserializer<std::unique_ptr<T>>
    {
        bool operator()(const rapidjson::Value& json, std::unique_ptr<T>& pointer) const
        {
            auto p = std::make_unique<T>();
            if (!deserialize(json, *p)) {
                return false;
            }

            pointer.swap(p);
            return true;
        }
    };

    template<typename T>
    struct JsonDeserializer<std::shared_ptr<T>>
    {
        bool operator()(const rapidjson::Value& json, std::shared_ptr<T>& pointer) const
        {
            auto p = std::make_shared<T>();
            if (!deserialize(json, *p)) {
                return false;
            }

            pointer.swap(p);
            return true;
        }
    };
}
