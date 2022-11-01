#pragma once
#include "deserialize_base.hpp"
#include "detail/deserialize_aware.hpp"
#include "detail/traits.hpp"
#include <rapidjson/error/en.h>
#include <string>

namespace persistence
{
    template<bool Exception, typename T>
    auto make_deserializer(DeserializerContext& context)
    {
        using value_type = typename unqualified<T>::type;
        using deserializer_type = JsonDeserializer<Exception, value_type>;
        return deserializer_type(context);
    }

    /**
     * Deserializes a C++ object from a JSON DOM representation.
     *
     * @tparam Exception True if exceptions may be thrown to signal deserialization failures.
     * @tparam T Type to be deserialized.
     */
    template<bool Exception, typename T>
    bool deserialize(const rapidjson::Value& json, T& obj, DeserializerContext& context)
    {
        auto deserializer = make_deserializer<Exception, typename unqualified<T>::type>(context);
        return deserializer(json, obj);
    }

    /**
     * Deserializes a C++ object from a JSON DOM document.
     */
    template<typename T>
    bool deserialize(rapidjson::Document& doc, T& obj)
    {
        if (doc.HasParseError()) {
            return false;
        }

        GlobalDeserializerContext global(doc);
        DeserializerContext local(global);
        return deserialize<false>(doc, obj, local);
    }

    /**
     * Deserializes a C++ object from a JSON DOM document.
     */
    template<typename T>
    T deserialize(rapidjson::Document& doc)
    {
        auto&& error = doc.GetParseError();
        if (error) {
            throw JsonParseError(rapidjson::GetParseError_En(error), doc.GetErrorOffset());
        }

        T obj;
        GlobalDeserializerContext global(doc);
        DeserializerContext local(global);
        deserialize<true>(doc, obj, local);
        return obj;
    }

    /**
     * Deserializes a C++ object from a JSON string via JSON DOM.
     *
     * Does not throw parse exceptions.
     *
     * @param str The source string.
     * @param obj A reference to an empty C++ object to populate.
     */
    template<typename T>
    bool deserialize(const std::string& str, T& obj)
    {
        rapidjson::Document doc;
        doc.Parse(str.data());
        auto&& error = doc.GetParseError();
        return !error && deserialize(doc, obj);
    }

    /**
     * Deserializes a C++ object from a JSON string via JSON DOM.
     *
     * Does not throw parse exceptions.
     *
     * @param str The source string.
     * @param obj A reference to an empty C++ object to populate.
     */
    template<typename T>
    bool deserialize(std::string&& str, T& obj)
    {
        rapidjson::Document doc;
        doc.ParseInsitu(str.data());
        auto&& error = doc.GetParseError();
        return !error && deserialize(doc, obj);
    }

    /**
     * Deserializes a C++ object from a JSON string via JSON DOM.
     */
    template<typename T>
    T deserialize(const std::string& str)
    {
        rapidjson::Document doc;
        doc.Parse(str.data());
        return deserialize<T>(doc);
    }

    /**
     * Deserializes a C++ object from a JSON string via JSON DOM.
     */
    template<typename T>
    T deserialize(std::string&& str)
    {
        rapidjson::Document doc;
        doc.ParseInsitu(str.data());
        return deserialize<T>(doc);
    }
}
