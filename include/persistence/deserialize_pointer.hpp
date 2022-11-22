#pragma once
#include "deserialize_base.hpp"
#include "detail/deserialize_aware.hpp"
#include "detail/unlikely.hpp"
#include <rapidjson/pointer.h>
#include <memory>

namespace persistence
{
    template<bool Exception, typename T>
    struct JsonDeserializer<Exception, T*>
    {
        static_assert(detail::fail<T>, "deserialization for raw pointers is not supported, use smart pointers instead");
    };

    template<bool Exception, typename T>
    struct JsonDeserializer<Exception, std::unique_ptr<T>> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, std::unique_ptr<T>& pointer) const
        {
            auto p = std::make_unique<T>();
            PERSISTENCE_IF_UNLIKELY(!deserialize<Exception>(json, *p, context)) {
                return false;
            }

            pointer.swap(p);
            return true;
        }
    };

    template<bool Exception, typename T>
    struct JsonDeserializer<Exception, std::shared_ptr<T>> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, std::shared_ptr<T>& pointer) const
        {
            if (json.IsObject()) {
                auto&& it = json.FindMember("$ref");
                if (it != json.MemberEnd()) {
                    rapidjson::Pointer path(it->value.GetString(), it->value.GetStringLength());
                    rapidjson::Value* object = path.Get(context.document());
                    PERSISTENCE_IF_UNLIKELY(!object) {
                        return false;
                    }

                    auto ref = context.get(object);
                    PERSISTENCE_IF_UNLIKELY(!ref.has_value()) {
                        return false;
                    }

                    pointer = std::reinterpret_pointer_cast<T>(ref.value());
                    return true;
                }
            }

            pointer = std::make_shared<T>();
            PERSISTENCE_IF_UNLIKELY(!deserialize<Exception>(json, *pointer, context)) {
                return false;
            }

            context.put(&json, pointer);
            return true;
        }
    };
}
