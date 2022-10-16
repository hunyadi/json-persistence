#pragma once
#include "write_base.hpp"
#include <string>
#include <string_view>

namespace persistence
{
    template<std::size_t N>
    struct JsonWriter<char[N]>
    {
        bool operator()(const char(&value)[N], StringWriter& writer) const
        {
            writer.String(value, N - 1, false);  // remove terminating NUL character
            return true;
        }
    };

    template<>
    struct JsonWriter<std::string_view>
    {
        bool operator()(const std::string_view& value, StringWriter& writer) const
        {
            writer.String(value.size() > 0 ? value.data() : "", value.size(), false);
            return true;
        }
    };

    template<>
    struct JsonWriter<std::string>
    {
        bool operator()(const std::string& value, StringWriter& writer) const
        {
            writer.String(value.data(), value.size(), true);
            return true;
        }
    };
}
