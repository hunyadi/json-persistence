#pragma once
#include <type_traits>

namespace persistence
{
    template<typename T>
    struct unqualified
    {
        using type = typename std::remove_reference<typename std::remove_volatile<T>::type>::type;
    };
}
