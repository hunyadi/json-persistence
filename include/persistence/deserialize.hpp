#pragma once
#include "deserialize_base.hpp"
#include "detail/deserialize_aware.hpp"
#include "detail/traits.hpp"
#include "exception.hpp"
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

    /**
     * Parses the JSON DOM representation of an object.
     */
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
     * Parses the JSON DOM representation of an object.
     */
    template<typename T>
    T deserialize(rapidjson::Document& doc)
    {
        if (doc.HasParseError()) {
            throw JsonDeserializationError();
        }

        T obj;
        if (!deserialize(doc, obj)) {
            throw JsonDeserializationError();
        }
        return obj;
    }

    /**
     * Parses the JSON string representation of an object.
     */
    template<typename T>
    bool deserialize(const std::string& str, T& obj)
    {
        rapidjson::Document doc;
        doc.Parse(str.data());
        auto&& error = doc.GetParseError();
        return !error && deserialize(doc, obj);
    }

    template<typename T>
    bool deserialize(std::string&& str, T& obj)
    {
        rapidjson::Document doc;
        doc.ParseInsitu(str.data());
        auto&& error = doc.GetParseError();
        return !error && deserialize(doc, obj);
    }

    template<typename T>
    T deserialize(const std::string& str)
    {
        T obj;
        rapidjson::Document doc;
        doc.Parse(str.data());
        auto&& error = doc.GetParseError();
        if (error) {
            throw JsonDeserializationError(doc.GetErrorOffset());
        }
        if (!deserialize(doc, obj)) {
            throw JsonDeserializationError();
        }
        return obj;
    }

    template<typename T>
    T deserialize(std::string&& str)
    {
        T obj;
        rapidjson::Document doc;
        doc.ParseInsitu(str.data());
        auto&& error = doc.GetParseError();
        if (error) {
            throw JsonDeserializationError(doc.GetErrorOffset());
        }
        if (!deserialize(doc, obj)) {
            throw JsonDeserializationError();
        }
        return obj;
    }
}
