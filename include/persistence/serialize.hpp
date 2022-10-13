#pragma once
#include "serialize_allocate.hpp"
#include "serialize_base.hpp"
#include "detail/traits.hpp"
#include <stdexcept>

namespace persistence
{
    template<typename T>
    auto make_serializer(rapidjson::MemoryPoolAllocator<>& allocator)
    {
        using value_type = typename unqualified<T>::type;
        using serializer_type = JsonSerializer<value_type>;

        if constexpr (std::is_base_of<JsonAllocatingSerializer, serializer_type>::value) {
            return serializer_type(allocator);
        } else {
            return serializer_type();
        }
    }

    /**
    * Generates the JSON representation of an object.
    */
    template<typename T>
    bool serialize(const T& obj, rapidjson::Value& json, rapidjson::MemoryPoolAllocator<>& allocator)
    {
        auto serializer = make_serializer<typename unqualified<T>::type>(allocator);
        return serializer(obj, json);
    }

    template<typename T>
    bool serialize(const T& obj, rapidjson::Writer<rapidjson::StringBuffer>& writer)
    {
        rapidjson::Document doc;
        bool result = serialize(obj, doc, doc.GetAllocator());
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
