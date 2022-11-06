#pragma once
#include <type_traits>

namespace persistence
{
    template<typename T>
    struct unqualified
    {
        using type = std::remove_reference_t<std::remove_volatile_t<T>>;
    };

    template<typename T>
    using unqualified_t = typename unqualified<T>::type;

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
