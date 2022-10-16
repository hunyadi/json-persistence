#pragma once
#include "deserialize_base.hpp"
#include "detail/deserialize_aware.hpp"
#include "detail/traits.hpp"
#include <stdexcept>
#include <string>

namespace persistence
{
    template<typename T>
    auto make_deserializer(DeserializerContext& context)
    {
        using value_type = typename unqualified<T>::type;
        using deserializer_type = JsonDeserializer<value_type>;

        if constexpr (std::is_base_of<JsonContextAwareDeserializer, deserializer_type>::value) {
            return deserializer_type(context);
        } else {
            return deserializer_type();
        }
    }

    /**
    * Parses the JSON representation of an object.
    */
    template<typename T>
    bool deserialize(const rapidjson::Value& json, T& obj, DeserializerContext& context)
    {
        auto deserializer = make_deserializer<typename unqualified<T>::type>(context);
        return deserializer(json, obj);
    }

    template<typename T>
    bool deserialize(rapidjson::Document& doc, T& obj)
    {
        if (doc.HasParseError()) {
            return false;
        } else {
            GlobalDeserializerContext global(doc);
            DeserializerContext local(global);
            return deserialize(doc, obj, local);
        }
    }

    /**
     * Parses the JSON representation of an object.
     */
    template<typename T>
    bool deserialize(const std::string& str, T& obj)
    {
        rapidjson::Document doc;
        doc.Parse(str.data());
        return deserialize(doc, obj);
    }

    template<typename T>
    bool deserialize(std::string&& str, T& obj)
    {
        rapidjson::Document doc;
        doc.ParseInsitu(str.data());
        return deserialize(doc, obj);
    }

    struct JsonDeserializationError : std::runtime_error
    {
        JsonDeserializationError() : std::runtime_error("deserialization failed") {}
    };

    template<typename T, typename S>
    T deserialize(S&& str)
    {
        T obj;
        if (!deserialize(std::forward<S>(str), obj)) {
            throw JsonDeserializationError();
        }
        return obj;
    }
}
