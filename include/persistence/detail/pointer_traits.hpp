#pragma once
#include <memory>
#include <type_traits>

namespace persistence
{
    template<typename T>
    struct is_smart_pointer : std::false_type
    {};

    template<typename T>
    struct is_smart_pointer<std::shared_ptr<T>> : std::true_type
    {};

    template<typename T>
    struct is_smart_pointer<std::unique_ptr<T>> : std::true_type
    {};

    template<typename T>
    struct is_pointer_like
    {
        static constexpr bool value = std::is_pointer<T>::value || is_smart_pointer<T>::value;
    };
}
