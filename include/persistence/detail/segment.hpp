#pragma once
#include <string_view>
#include <vector>

namespace persistence
{
    struct Segment
    {
        Segment() = default;
        Segment(const std::string_view& name) : name(name) {}
        Segment(std::size_t index) : index(index) {}

        std::string_view name;
        std::size_t index = 0;

        bool is_named() const { return name.size() > 0; }
        bool is_indexed() const { return name.size() == 0; }
    };

    using Segments = std::vector<Segment>;
}
