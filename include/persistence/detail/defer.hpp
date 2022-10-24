#pragma once
#include <type_traits>

namespace persistence
{
    namespace detail
    {
        /**
        * Utility template to prevent immediate evaluation of an expression in a `static_assert`.
        */
        template<typename T>
        struct defer : public std::false_type
        {};
    }
}
