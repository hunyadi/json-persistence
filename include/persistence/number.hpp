#pragma once
#include "detail/defer.hpp"
#include <charconv>
#include <cstdlib>
#include <string_view>

namespace persistence
{
    namespace detail
    {
        template<typename T>
        bool parse_from_chars(const std::string_view& str, T& value)
        {
            const char* last = str.data() + str.size();
            auto result = std::from_chars(str.data(), last, value);
            return result.ec == std::errc() && result.ptr == last;
        }
    }

    template<typename T, typename Enable = void>
    struct NumberParser
    {
        static_assert(detail::fail<T>, "unrecognized number type");
    };

    template<typename T>
    struct NumberParser<T, std::enable_if_t<std::is_integral_v<T>>>
    {
        bool parse(const std::string_view& str, T& value)
        {
            return detail::parse_from_chars(str, value);
        }
    };

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    template<typename T>
    struct NumberParser<T, std::enable_if_t<std::is_floating_point_v<T>>>
    {
        bool parse(const std::string_view& str, T& value)
        {
            return detail::parse_from_chars(str, value);
        }
    };
#else
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

    template<>
    struct NumberParser<long double>
    {
        bool parse(const std::string_view& str, long double& value)
        {
            char* last;
            value = std::strtold(str.data(), &last);
            return last == str.data() + str.size();
        }
    };
#endif

    template<typename T>
    bool parse_number(const std::string_view& str, T& value)
    {
        NumberParser<T> parser;
        return parser.parse(str, value);
    }
}
