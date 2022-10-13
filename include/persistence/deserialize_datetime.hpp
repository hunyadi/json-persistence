#pragma once
#include "deserialize_base.hpp"
#include "datetime.hpp"
#include <charconv>
#include <chrono>
#include <string>

namespace persistence
{
    namespace detail
    {
        inline bool from_digits(const char* first, const char* last, int& value) {
            auto result = std::from_chars(first, last, value);
            return result.ec == std::errc();
        }
    }

    template<>
    struct JsonDeserializer<timestamp>
    {
        bool operator()(const rapidjson::Value& json, timestamp& value) const
        {
            if (!json.IsString()) {
                return false;
            }

            if (json.GetStringLength() != 20) {
                return false;
            }

            auto text = json.GetString();
            if (text[4] != '-' ||
                text[7] != '-' ||
                (text[10] != 'T' && text[10] != ' ') ||
                text[13] != ':' ||
                text[16] != ':' ||
                text[19] != 'Z'
            ) {
                return false;
            }

            using detail::from_digits;
            int year, month, day, hour, minute, second;
            if (!from_digits(text, text + 4, year) ||
                !from_digits(text + 5, text + 7, month) ||
                !from_digits(text + 8, text + 10, day) ||
                !from_digits(text + 11, text + 13, hour) ||
                !from_digits(text + 14, text + 16, minute) ||
                !from_digits(text + 17, text + 19, second)
            ) {
                return false;
            }

            value = make_datetime(year, month, day, hour, minute, second);
            return true;
        }
    };
}
