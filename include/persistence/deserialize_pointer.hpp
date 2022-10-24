#pragma once
#include "deserialize_base.hpp"
#include "detail/deserialize_aware.hpp"
#include <rapidjson/pointer.h>

namespace persistence
{
    template<typename T>
    struct JsonDeserializer<T*>
    {
        static_assert(
            detail::defer<T>::value,
            "deserialization for raw pointers is not supported, use smart pointers instead"
        );
    };

    template<typename T>
    struct JsonDeserializer<std::unique_ptr<T>> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, std::unique_ptr<T>& pointer) const
        {
            auto p = std::make_unique<T>();
            if (!deserialize(json, *p, context)) {
                return false;
            }

            pointer.swap(p);
            return true;
        }
    };

    template<typename T>
    struct JsonDeserializer<std::shared_ptr<T>> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, std::shared_ptr<T>& pointer) const
        {
            if (json.IsObject()) {
                auto&& it = json.FindMember("$ref");
                if (it != json.MemberEnd()) {
                    rapidjson::Pointer path(it->value.GetString(), it->value.GetStringLength());
                    rapidjson::Value* object = path.Get(context.document());
                    if (!object) {
                        return false;
                    }

                    pointer = std::reinterpret_pointer_cast<T>(context.get(object));
                    if (!pointer) {
                        return false;
                    }

                    return true;
                }
            }

            auto p = std::make_shared<T>();
            if (!deserialize(json, *p, context)) {
                return false;
            }

            context.put(&json, p);
            pointer.swap(p);
            return true;
        }
    };
}
