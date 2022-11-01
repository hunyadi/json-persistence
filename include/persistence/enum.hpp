#pragma once
#include <string_view>
#include <type_traits>

namespace persistence
{
    /**
    * Generic template for enumeration traits.
    *
    * Define template specializations with functions `to_string` and `from_string` to allow
    * (de-)serializing enumeration types to/from strings:
    * ```
    * static std::string_view to_string(Enum) { ... }
    * static bool from_string(const std::string_view&, Enum&) { ... }
    * ```
    *
    * @tparam An enumeration type.
    */
    template<typename Enum, typename Enable = void>
    struct enum_traits
    {
        static_assert(std::is_enum<Enum>::value, "expected an enumeration type");
    };

    template<typename T>
    using enum_to_string_function = decltype(enum_traits<T>::to_string(std::declval<T>()));

    template<typename T>
    using enum_from_string_function = decltype(enum_traits<T>::from_string(std::declval<const std::string_view&>(), std::declval<T&>()));
}
