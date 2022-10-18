#pragma once
#include <string_view>
#include <type_traits>

/**
* Helps enumerate member variables in a persistence template function.
*/
#define NAMED_MEMBER_VARIABLE(name, variable) \
    ::persistence::member_variable<decltype(variable), std::remove_pointer_t<decltype(this)>>(name, &std::remove_reference_t<decltype(*this)>::variable)

#define MEMBER_VARIABLE(variable) \
    NAMED_MEMBER_VARIABLE(#variable, variable)

namespace persistence
{
    /**
    * Represents a member variable in a (de-)serialization declaration.
    * @tparam Type The type of the member variable bound by this structure.
    * @tparam Class The type of the encapsulating object.
    */
    template<typename Type, typename Class>
    struct member_variable
    {
        constexpr member_variable(const std::string_view& name, Type Class::* member_pointer)
            : name(name)
            , member_pointer(member_pointer)
        {}

        /**
        * The name of the member variable as a constant string literal.
        */
        std::string_view name;

        /**
        * A writable reference to the member variable value.
        */
        Type& ref(Class& obj) const
        {
            return obj.*member_pointer;
        }

        /**
        * A read-only reference to the member variable value.
        */
        const Type& ref(const Class& obj) const
        {
            return obj.*member_pointer;
        }

        Type Class::* member_pointer;
    };
}
