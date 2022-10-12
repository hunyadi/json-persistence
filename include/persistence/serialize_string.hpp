#pragma once
#include "detail/engine.hpp"
#include "serialize_allocate.hpp"
#include "serialize_base.hpp"
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
            json.SetString(value.size() > 0 ? value.data() : "", value.size());
            return true;
        }
    };

    template<>
    struct JsonSerializer<std::string> : JsonAllocatingSerializer
    {
        using JsonAllocatingSerializer::JsonAllocatingSerializer;

        bool operator()(const std::string& value, rapidjson::Value& json) const
        {
            json.SetString(value.data(), value.size(), allocator);
            return true;
        }
    };
}