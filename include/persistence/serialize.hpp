#pragma once
#include "serialize_base.hpp"
#include "detail/serialize_aware.hpp"
#include "detail/traits.hpp"
#include "exception.hpp"

namespace persistence
{
    template<typename T>
    auto make_serializer(SerializerContext& context)
    {
        using serializer_type = JsonSerializer<unqualified_t<T>>;
        if constexpr (std::is_base_of<JsonContextAwareSerializer, serializer_type>::value) {
            return serializer_type(context);
        } else {
            return serializer_type();
        }
    }

    /**
     * Generates the JSON representation of an object.
     */
    template<typename T>
    bool serialize(const T& obj, rapidjson::Value& json, SerializerContext& context)
    {
        auto json_serializer = make_serializer<unqualified_t<T>>(context);
        return json_serializer(obj, json);
    }

    /**
     * Serializes an object into a JSON DOM representation.
     */
    template<typename T>
    bool serialize_to_document(const T& obj, rapidjson::Document& doc)
    {
        GlobalSerializerContext global(doc);
        SerializerContext local(global);
        return serialize(obj, doc, local);
    }

    /**
     * Serializes an object into a JSON DOM representation.
     */
    template<typename T>
    rapidjson::Document serialize_to_document(const T& obj)
    {
        rapidjson::Document doc;
        if (!serialize_to_document(obj, doc)) {
            throw JsonSerializationError();
        }
        return doc;
    }
}
