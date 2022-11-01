#pragma once
#include "enum.hpp"
#include "write_fundamental.hpp"
#include "write_string.hpp"

namespace persistence
{
    template<typename T>
    struct JsonWriter<T, typename std::enable_if<std::is_enum<T>::value>::type>
    {
        bool operator()(T value, StringWriter& writer) const
        {
            if constexpr (detect<T, enum_to_string_function>::value) {
                static_assert(
                    std::is_same_v<enum_to_string_function<T>, std::string_view>,
                    "expected function with signature: static std::string_view to_string(Enum)"
                );
                JsonWriter<std::string_view> string_writer;
                return string_writer(enum_traits<T>::to_string(value), writer);
            } else {
                using integer_type = typename std::underlying_type<T>::type;
                JsonWriter<integer_type> integer_writer;
                return integer_writer(static_cast<integer_type>(value), writer);
            }
        }
    };
}
