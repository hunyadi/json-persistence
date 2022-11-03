#pragma once
#include <type_traits>

namespace persistence
{
    namespace detail
    {
        /**
         * Utility template to prevent immediate evaluation of an expression in a `static_assert`.
         *
         * The following would be immediately evaluated in a template definition, and thus fail to compile:
         * ```
         * static_assert(false, "...");
         * ```
         *
         * The following alternative defers evaluation; it allows a general template class with the assertion
         * to be specialized, and cause the compiler to fail only when none of the specializations match:
         * ```
         * static_assert(detail::defer<T>::value, "...");
         * ```
         */
        template<typename T>
        struct defer : public std::false_type
        {};
    }
}
