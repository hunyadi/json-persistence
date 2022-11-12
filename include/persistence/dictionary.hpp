#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <unordered_map>

namespace persistence
{
    template<typename T>
    using literal_dict = std::vector<std::pair<std::string_view, T>>;

    template<typename T>
    using literal_map = std::map<std::string_view, T>;

    template<typename T>
    using literal_unordered_map = std::unordered_map<std::string_view, T>;

    template<typename T>
    using string_dict = std::vector<std::pair<std::string, T>>;

    template<typename T>
    using string_map = std::map<std::string, T>;

    template<typename T>
    using string_unordered_map = std::unordered_map<std::string, T>;
}
