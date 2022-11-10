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
        using writer_type = JsonWriter<unqualified_t<T>>;
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
        auto serializer = make_writer<unqualified_t<T>>(context);
        return serializer(obj, writer);
    }

    template<typename T>
    bool write_to_string(const T& obj, StringWriter& writer)
    {
        GlobalWriterContext global;
        WriterContext local(global);
        return serialize(obj, writer, local);
    }

    /**
     * Produces the JSON string representation of an object.
     */
    template<typename T>
    bool write_to_string(const T& obj, std::string& str)
    {
        rapidjson::StringBuffer buffer;
        StringWriter writer(buffer);
        bool result = write_to_string(obj, writer);
        if (result) {
            str.assign(buffer.GetString(), buffer.GetString() + buffer.GetSize());
            return true;
        } else {
            return false;
        }
    }

    /**
     * Produces the JSON string representation of an object.
     */
    template<typename T>
    std::string write_to_string(const T& obj)
    {
        std::string str;
        if (!write_to_string(obj, str)) {
            throw JsonSerializationError();
        }
        return str;
    }
}
