#pragma once
#include "serialize_base.hpp"
#include "detail/serialize_aware.hpp"
#include <string>
#include <string_view>

namespace persistence
{
    template<std::size_t N>
    struct JsonSerializer<char[N]>
    {
        bool operator()(const char(&value)[N], rapidjson::Value& json) const
        {
            json.SetString(value, N - 1);  // remove terminating NUL character
            return true;
        }
    };

    template<>
    struct JsonSerializer<std::string_view>
    {
        bool operator()(const std::string_view& value, rapidjson::Value& json) const
        {
            json.SetString(
                value.size() > 0 ? value.data() : "",
                static_cast<rapidjson::SizeType>(value.size())
            );
            return true;
        }
    };

    template<>
    struct JsonSerializer<std::string> : JsonContextAwareSerializer
    {
        using JsonContextAwareSerializer::JsonContextAwareSerializer;

        bool operator()(const std::string& value, rapidjson::Value& json) const
        {
            json.SetString(value.data(), static_cast<rapidjson::SizeType>(value.size()), context.global().allocator());
            return true;
        }
    };
}
