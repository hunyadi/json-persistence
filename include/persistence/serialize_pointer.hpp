#pragma once
#include "serialize_base.hpp"
#include "detail/serialize_aware.hpp"
#include "detail/path.hpp"

namespace persistence
{
    template<typename T>
    struct JsonSerializer<std::unique_ptr<T>> : JsonContextAwareSerializer
    {
        using JsonContextAwareSerializer::JsonContextAwareSerializer;

        bool operator()(const std::unique_ptr<T>& pointer, rapidjson::Value& json) const
        {
            return serialize(*pointer, json, context);
        }
    };

    template<typename T>
    struct JsonSerializer<std::shared_ptr<T>> : JsonContextAwareSerializer
    {
        using JsonContextAwareSerializer::JsonContextAwareSerializer;

        bool operator()(const std::shared_ptr<T>& pointer, rapidjson::Value& json) const
        {
            auto&& segments = context.get(pointer);
            if (!segments.empty()) {
                std::string ref = Path(segments).str();

                // create a JSON string "/path/to/earlier/occurrence"
                rapidjson::Value ref_json;
                ref_json.SetString(ref.data(), ref.size(), context.global().allocator());

                // create a {"$ref": "/path/to/earlier/occurrence"}
                json.SetObject();
                rapidjson::Value::StringRefType ref_key("$ref");
                json.AddMember(ref_key, ref_json, context.global().allocator());

            } else {
                if (!serialize(*pointer, json, context)) {
                    return false;
                }

                // assign a "/path/to/this/occurrence"
                context.put(pointer);
            }
            return true;
        }
    };
}
