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
        auto serializer = make_serializer<typename unqualified<T>::type>(context);
        return serializer(obj, json);
    }

    template<typename T>
    bool serialize(const T& obj, rapidjson::Writer<rapidjson::StringBuffer>& writer)
    {
        rapidjson::Document doc;
        GlobalSerializerContext global(doc);
        SerializerContext local(global);
        bool result = serialize(obj, doc, local);
        if (result) {
            doc.Accept(writer);
            return true;
        } else {
            return false;
        }
    }

    /**
    * Generates the JSON representation of an object.
    */
    template<typename T>
    bool serialize(const T& obj, std::string& str)
    {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        bool result = serialize(obj, writer);
        if (result) {
            str.assign(buffer.GetString(), buffer.GetString() + buffer.GetSize());
            return true;
        } else {
            return false;
        }
    }

    struct JsonSerializationError : std::runtime_error
    {
        JsonSerializationError() : std::runtime_error("serialization failed") {}
    };

    template<typename T>
    std::string serialize_to_string(const T& obj)
    {
        std::string str;
        if (!serialize(obj, str)) {
            throw JsonSerializationError();
        }
        return str;
    }
}
