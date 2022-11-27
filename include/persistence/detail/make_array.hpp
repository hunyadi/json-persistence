#pragma once
#include <array>

namespace persistence
{
    namespace detail
    {
        template<typename> struct is_ref_wrapper : std::false_type
        {};

        template<typename T> struct is_ref_wrapper<std::reference_wrapper<T>> : std::true_type
        {};

        template<typename T>
        using not_ref_wrapper = std::negation<is_ref_wrapper<std::decay_t<T>>>;

        template<typename D, typename...>
        struct return_type_helper
        {
            using type = D;
        };

        template<typename... Types>
        struct return_type_helper<void, Types...> : std::common_type<Types...>
        {
            static_assert(std::conjunction_v<not_ref_wrapper<Types>...>,
                "types cannot contain reference wrappers when D is void");
        };

        template<typename D, typename... Types>
        using return_type = std::array<typename return_type_helper<D, Types...>::type, sizeof...(Types)>;
    }

    template<typename D = void, typename... Types>
    constexpr detail::return_type<D, Types...> make_array(Types&&... t)
    {
        return { std::forward<Types>(t)... };
    }
}
