#pragma once
#include <string_view>
#include <vector>

namespace persistence
{
    /**
     * A segment in a JSON Path expression.
     *
     * Segments of a path are strings for object property access and integers for array index access.
     */
    struct Segment
    {
        Segment() = default;
        Segment(const std::string_view& name) : name(name) {}
        Segment(std::size_t index) : index(index) {}

        std::string_view name;
        std::size_t index = 0;

        /** True for object property access. */
        bool is_named() const { return name.size() > 0; }

        /** True for array index access. */
        bool is_indexed() const { return name.size() == 0; }
    };

    using Segments = std::vector<Segment>;
}
