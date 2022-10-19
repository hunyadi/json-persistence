#pragma once
#include "write_base.hpp"
#include "detail/write_aware.hpp"
#include "detail/traits.hpp"
#include "exception.hpp"

namespace persistence
{
    template<typename T>
    auto make_writer(WriterContext& context)
    {
        using value_type = typename unqualified<T>::type;
        using writer_type = JsonWriter<value_type>;

        if constexpr (std::is_base_of<JsonContextAwareWriter, writer_type>::value) {
            return writer_type(context);
        } else {
            return writer_type();
        }
    }

    /**
    * Writes an object to a JSON string.
    */
    template<typename T>
    bool serialize(const T& obj, StringWriter& writer, WriterContext& context)
    {
        auto serializer = make_writer<typename unqualified<T>::type>(context);
        return serializer(obj, writer);
    }

    template<typename T>
    bool serialize_to_string(const T& obj, StringWriter& writer)
    {
        GlobalWriterContext global;
        WriterContext local(global);
        return serialize(obj, writer, local);
    }

    /**
    * Generates the JSON string representation of an object.
    */
    template<typename T>
    bool serialize_to_string(const T& obj, std::string& str)
    {
        rapidjson::StringBuffer buffer;
        StringWriter writer(buffer);
        bool result = serialize_to_string(obj, writer);
        if (result) {
            str.assign(buffer.GetString(), buffer.GetString() + buffer.GetSize());
            return true;
        } else {
            return false;
        }
    }

    template<typename T>
    std::string serialize_to_string(const T& obj)
    {
        std::string str;
        if (!serialize_to_string(obj, str)) {
            throw JsonSerializationError();
        }
        return str;
    }
}
