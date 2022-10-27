#pragma once
#include <cstddef>
#include <string_view>
#include <vector>

namespace persistence
{
    using byte_vector = std::vector<std::byte>;
    using byte_view = std::basic_string_view<std::byte>;

    template<typename... Ts>
    byte_vector make_byte_vector(Ts&&... args) noexcept {
        return { std::byte(std::forward<Ts>(args))... };
    }
}
