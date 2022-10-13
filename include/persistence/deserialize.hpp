#pragma once
#include "deserialize_base.hpp"
#include "detail/traits.hpp"
#include <stdexcept>
#include <string>

namespace persistence
{
    /**
    * Parses the JSON representation of an object.
    */
    template<typename T>
    bool deserialize(const rapidjson::Value& json, T& obj)
    {
        JsonDeserializer<typename unqualified<T>::type> deserializer;
        return deserializer(json, obj);
    }

    /**
     * Parses the JSON representation of an object.
     */
    template<typename T>
    bool deserialize(const std::string& str, T& obj)
    {
        rapidjson::Document doc;
        doc.Parse(str.c_str());
        if (doc.HasParseError()) {
            return false;
        } else {
            return deserialize(doc, obj);
        }
    }

    struct JsonDeserializationError : std::runtime_error
    {
        JsonDeserializationError() : std::runtime_error("deserialization failed") {}
    };

    template<typename T>
    T deserialize(const std::string& str)
    {
        T obj;
        if (!deserialize(str, obj)) {
            throw JsonDeserializationError();
        }
        return obj;
    }
}
