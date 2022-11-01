#pragma once
#include "segment.hpp"
#include <charconv>

namespace persistence
{
    namespace detail
    {
        constexpr std::size_t digit_count(std::size_t value)
        {
            if (value < 10)
                return 1;
            else if (value < 100)
                return 2;
            else if (value < 1000)
                return 3;
            else if (value < 10000)
                return 4;
            else if (value < 100000)
                return 5;
            else if (value < 1000000)
                return 6;
            else if (value < 10000000)
                return 7;
            else if (value < 100000000)
                return 8;
            else if (value < 1000000000)
                return 9;
            else if (value < 10000000000)
                return 10;
            else if (value < 100000000000)
                return 11;
            else if (value < 1000000000000)
                return 12;
            else if (value < 10000000000000)
                return 13;
            else if (value < 100000000000000)
                return 14;
            else if (value < 1000000000000000)
                return 15;
            else if (value < 10000000000000000)
                return 16;
            else if (value < 100000000000000000)
                return 17;
            else if (value < 1000000000000000000)
                return 18;
            else
                return 19;
        }
    }

    /**
    * A JSON Path expression.
    *
    * Segments of a path are strings for object property access and integers for array index access.
    */
    struct Path
    {
        Path() = default;
        Path(Segments segments) : segments(segments) {}

        std::string str() const
        {
            std::string path;
            for (auto&& it = segments.rbegin(); it != segments.rend(); ++it) {
                path.push_back('/');
                if (it->is_named()) {
                    path.append(it->name);
                } else {
                    std::size_t off = path.length();
                    std::size_t len = detail::digit_count(it->index);
                    path.resize(path.size() + len);
                    std::to_chars(path.data() + off, path.data() + off + len, it->index);
                }
            }
            return path;
        }

    private:
        Segments segments;
    };
}
