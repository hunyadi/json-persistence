#pragma once
#include <string_view>

/**
* Helps enumerate member variables in a persistence template function.
*/
#define NAMED_MEMBER_VARIABLE(name, variable) ::persistence::member_variable<decltype(variable)>(name, variable)
#define MEMBER_VARIABLE(variable) NAMED_MEMBER_VARIABLE(#variable, variable)

namespace persistence
{
    /**
    * Represents a member variable in a (de-)serialization declaration.
    * @tparam T The type of the member variable bound by this structure.
    */
    template<typename T>
    struct member_variable
    {
        member_variable(const std::string_view& name, T& ref)
            : name(name)
            , ref(ref)
        {}

        /**
        * The name of the member variable as a constant string literal.
        */
        std::string_view name;

        /**
        * A writable reference to the member variable value.
        */
        T& ref;
    };
}
