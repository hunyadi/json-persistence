#pragma once
#include <type_traits>

namespace persistence
{
    template<typename T>
    struct unqualified
    {
        using type = typename std::remove_reference<typename std::remove_volatile<T>::type>::type;
    };

    /** Checks if a class supports an operation. */
    template<typename, template <typename> class, typename = void>
    struct detect : std::false_type
    {
        // primary template handles all types not supporting the operation
    };

    template<typename T, template <typename> class Op>
    struct detect<T, Op, std::void_t<Op<T>>> : std::true_type
    {
        // specialization recognizes/validates only types supporting the archetype
    };
}
