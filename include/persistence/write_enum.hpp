#pragma once
#include "write_fundamental.hpp"

namespace persistence
{
    template<typename T>
    struct JsonWriter<T, typename std::enable_if<std::is_enum<T>::value>::type>
    {
        bool operator()(T value, StringWriter& writer) const
        {
            using integer_type = typename std::underlying_type<T>::type;
            JsonWriter<integer_type> integer_writer;
            return integer_writer(static_cast<integer_type>(value), writer);
        }
    };
}
