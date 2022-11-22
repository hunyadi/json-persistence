#pragma once
#include "segment.hpp"
#include <charconv>

namespace persistence
{
    /**
     * A JSON Path expression.
     *
     * Segments of a path are strings for object property access and integers for array index access.
     */
    struct Path
    {
        Path() = default;
        Path(Segments&& segments) : segments(std::move(segments)) {}

        std::string str() const
        {
            if (segments.empty()) {
                return "/";
            }

            std::string path;
            for (auto&& it = segments.rbegin(); it != segments.rend(); ++it) {
                path.push_back('/');
                if (it->is_named()) {
                    path.append(it->name);
                } else {
                    // increase buffer by number of characters required to represent largest integer in decimal digits
                    constexpr std::size_t max_len = std::numeric_limits<std::size_t>::digits10;
                    std::size_t size = path.size();
                    path.resize(size + max_len);

                    // write numeric digits at end of buffer
                    auto result = std::to_chars(path.data() + size, path.data() + size + max_len, it->index);
                    if (result.ec == std::errc()) {
                        size = static_cast<std::size_t>(result.ptr - path.data());
                    }

                    // shrink buffer to real size
                    path.resize(size);
                }
            }
            return path;
        }

    private:
        Segments segments;
    };
}
