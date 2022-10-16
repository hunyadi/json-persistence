#pragma once
#include "serialize_base.hpp"
#include "detail/serialize_aware.hpp"
#include "detail/traits.hpp"
#include <stdexcept>

namespace persistence
{
    template<typename T>
    auto make_serializer(SerializerContext& context)
    {
        using value_type = typename unqualified<T>::type;
        using serializer_type = JsonSerializer<value_type>;

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
        auto json_serializer = make_serializer<typename unqualified<T>::type>(context);
        return json_serializer(obj, json);
    }

    /**
    * Generates the JSON DOM representation of an object.
    */
    template<typename T>
    bool serialize_to_document(const T& obj, rapidjson::Document& doc)
    {
        GlobalSerializerContext global(doc);
        SerializerContext local(global);
        return serialize(obj, doc, local);
    }
}
