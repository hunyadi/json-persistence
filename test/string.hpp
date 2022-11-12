#pragma once
#include <string_view>

namespace test
{
    /** Creates a string view from a constant string literal even if it contains NUL characters. */
    template<std::size_t N>
    constexpr std::string_view view_from_chars(const char(&s)[N])
    {
        return std::string_view(s, N - 1);
    }
}
