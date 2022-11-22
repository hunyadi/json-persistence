#pragma once
#include "serialize_base.hpp"
#include "detail/serialize_aware.hpp"
#include "detail/unlikely.hpp"
#include "detail/path.hpp"
#include <memory>

namespace persistence
{
    template<typename P>
    struct JsonPointerSerializer : JsonContextAwareSerializer
    {
        using JsonContextAwareSerializer::JsonContextAwareSerializer;

        bool operator()(const P& pointer, rapidjson::Value& json) const
        {
            return serialize(*pointer, json, context);
        }
    };

    template<typename T>
    struct JsonSerializer<T*> : JsonPointerSerializer<T*>
    {
        using JsonPointerSerializer<T*>::JsonPointerSerializer;
    };

    template<typename T>
    struct JsonSerializer<std::unique_ptr<T>> : JsonPointerSerializer<std::unique_ptr<T>>
    {
        using JsonPointerSerializer<std::unique_ptr<T>>::JsonPointerSerializer;
    };

    template<typename T>
    struct JsonSerializer<std::shared_ptr<T>> : JsonContextAwareSerializer
    {
        using JsonContextAwareSerializer::JsonContextAwareSerializer;

        bool operator()(const std::shared_ptr<T>& pointer, rapidjson::Value& json) const
        {
            auto segments = context.get(pointer);
            if (segments) {
                std::string ref = Path(std::move(*segments)).str();

                // create a JSON string "/path/to/earlier/occurrence"
                rapidjson::Value ref_json;
                ref_json.SetString(ref.data(), static_cast<rapidjson::SizeType>(ref.size()), context.global().allocator());

                // create a {"$ref": "/path/to/earlier/occurrence"}
                json.SetObject();
                rapidjson::Value::StringRefType ref_key("$ref");
                json.AddMember(ref_key, ref_json, context.global().allocator());

            } else {
                PERSISTENCE_IF_UNLIKELY(!serialize(*pointer, json, context)) {
                    return false;
                }

                // assign a "/path/to/this/occurrence"
                context.put(pointer);
            }
            return true;
        }
    };
}
