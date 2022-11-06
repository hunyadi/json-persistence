#pragma once
#include <array>

namespace persistence
{
    namespace detail
    {
        template<typename T, std::size_t N, std::size_t... I>
        constexpr std::array<std::remove_cv_t<T>, N> to_array_impl(T (&a)[N], std::index_sequence<I...>)
        {
            return { {a[I]...} };
        }

        /**
         * Creates an `std::array` instance from a classic one-dimensional array.
         *
         * Equivalent to `std::to_array` in C++20 and later.
         */
        template<typename T, std::size_t N>
        constexpr std::array<std::remove_cv_t<T>, N> to_array(T(&a)[N])
        {
            return detail::to_array_impl(a, std::make_index_sequence<N>{});
        }
    }

    /**
     * Creates a converter that maps enumeration values to strings and vice versa.
     *
     * @tparam E An enumeration type.
     * @tparam N The number of elements in the enumeration.
     */
    template<typename E, std::size_t N>
    struct EnumConverter
    {
        static_assert(std::is_enum_v<E>, "expected an enumeration type");
        static_assert(N >= 1, "expected at least one enumeration value");

        constexpr EnumConverter(std::array<std::pair<E, std::string_view>, N> value_to_name)
            : value_to_name(value_to_name)
        {}

        /** Generates the string representation of an enumeration value. */
        std::string_view to_string(E value) const
        {
            for (auto&& item : value_to_name) {
                if (item.first == value) {
                    return item.second;
                }
            }
            return std::string_view();
        }

        /** Parses the string representation of an enumeration value. */
        bool from_string(const std::string_view& name, E& value) const
        {
            for (auto&& item : value_to_name) {
                if (item.second == name) {
                    value = item.first;
                    return true;
                }
            }
            return false;
        }

    private:
        std::array<std::pair<E, std::string_view>, N> value_to_name;
    };

    template<typename E, std::size_t N>
    constexpr std::pair<E, std::string_view> make_enum(E e, const char (&s)[N])
    {
        return std::make_pair(e, std::string_view(s, N - 1));
    }

    template<typename E>
    constexpr std::pair<E, std::string_view> make_enum(E e, std::string_view s)
    {
        return std::make_pair(e, s);
    }

    template<typename E, std::size_t N>
    constexpr auto make_enum_converter(std::array<std::pair<E, std::string_view>, N> value_to_name)
    {
        return EnumConverter<E, N>(value_to_name);
    }

    template<typename E, std::size_t N>
    constexpr auto make_enum_converter(const std::pair<E, std::string_view> (&value_to_name)[N])
    {
        return EnumConverter<E, N>(detail::to_array(value_to_name));
    }
}
