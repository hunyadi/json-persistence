#pragma once
#include <charconv>
#include <cstdlib>
#include <string_view>

namespace persistence
{
    template<typename T, typename Enable = void>
    struct NumberParser
    {};

    template<typename T>
    struct NumberParser<T, typename std::enable_if<std::is_integral<T>::value>::type>
    {
        bool parse(const std::string_view& str, T& value)
        {
            const char* last = str.data() + str.size();
            auto result = std::from_chars(str.data(), last, value);
            return result.ec == std::errc() && result.ptr == last;
        }
    };

    template<>
    struct NumberParser<float>
    {
        bool parse(const std::string_view& str, float& value)
        {
            char* last;
            value = std::strtof(str.data(), &last);
            return last == str.data() + str.size();
        }
    };

    template<>
    struct NumberParser<double>
    {
        bool parse(const std::string_view& str, double& value)
        {
            char* last;
            value = std::strtod(str.data(), &last);
            return last == str.data() + str.size();
        }
    };

    template<typename T>
    bool parse_number(std::string_view str, T& value)
    {
        NumberParser<T> parser;
        return parser.parse(str, value);
    }
}
