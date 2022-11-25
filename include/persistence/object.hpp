#pragma once
#include <string_view>
#include <type_traits>

/**
 * Helps enumerate member variables in a persistence template function.
 */
#define NAMED_MEMBER_VARIABLE(name, variable) \
    ::persistence::class_member_variable<&std::remove_reference_t<decltype(*this)>::variable>(name)

/**
 * Helps enumerate member variables in a persistence template function.
 */
#define MEMBER_VARIABLE(variable) \
    NAMED_MEMBER_VARIABLE(#variable, variable)

/**
 * Helps enumerate member variables in a persistence template function.
 */
#define NAMED_MEMBER_VARIABLE_DEFAULT(name, variable) \
    ::persistence::class_member_variable_default<&std::remove_reference_t<decltype(*this)>::variable>(name)

/**
 * Helps enumerate member variables in a persistence template function.
 */
#define MEMBER_VARIABLE_DEFAULT(variable) \
    NAMED_MEMBER_VARIABLE_DEFAULT(#variable, variable)

namespace persistence
{
    template<typename Pointer>
    struct member_pointer_traits
    {};

    template<class Class, typename Type>
    struct member_pointer_traits<Type Class::*>
    {
        using member_type = Type;
        using class_type = Class;
    };

    /**
     * Represents a member variable in a (de-)serialization declaration.
     * @tparam Type The type of the member variable bound by this structure.
     * @tparam Class The type of the encapsulating object.
     * @tparam Pointer The member pointer value.
     */
    template<typename Type, class Class, Type Class::* Pointer>
    struct member_variable_declaration
    {
        using pointer_type = decltype(Pointer);
        /** The type of the member variable bound by this structure. */
        using member_type = Type;
        /** The type of the encapsulating object. */
        using class_type = Class;

        constexpr member_variable_declaration(const std::string_view& name)
            : member_name(name)
        {}

        /**
         * The name of the member variable as a constant string literal.
         */
        constexpr const std::string_view& name() const
        {
            return member_name;
        }

        /**
         * The member variable pointer. 
         */
        constexpr pointer_type pointer() const
        {
            return Pointer;
        }

        /**
         * A writable reference to the member variable value.
         */
        member_type& ref(class_type& obj) const
        {
            return obj.*Pointer;
        }

        /**
         * A read-only reference to the member variable value.
         */
        const member_type& ref(const class_type& obj) const
        {
            return obj.*Pointer;
        }

    private:
        std::string_view member_name;
    };
    
    template<typename Type, class Class, auto Pointer>
    struct member_variable : member_variable_declaration<Type, Class, Pointer>
    {
        using member_variable_declaration<Type, Class, Pointer>::member_variable_declaration;
    };

    template<typename Type, class Class, auto Pointer>
    struct member_variable_default : member_variable_declaration<Type, Class, Pointer>
    {
        using member_variable_declaration<Type, Class, Pointer>::member_variable_declaration;
    };

    template<auto MemberPointer>
    constexpr auto class_member_variable(const std::string_view& name)
    {
        using pointer_type = decltype(MemberPointer);
        static_assert(std::is_member_object_pointer_v<pointer_type>, "expected a non-static member object pointer");
        using Type = typename member_pointer_traits<pointer_type>::member_type;
        using Class = typename member_pointer_traits<pointer_type>::class_type;
        return member_variable<Type, Class, MemberPointer>(name);
    }

    template<auto MemberPointer>
    constexpr auto class_member_variable_default(const std::string_view& name)
    {
        using pointer_type = decltype(MemberPointer);
        static_assert(std::is_member_object_pointer_v<pointer_type>, "expected a non-static member object pointer");
        using Type = typename member_pointer_traits<pointer_type>::member_type;
        using Class = typename member_pointer_traits<pointer_type>::class_type;
        return member_variable_default<Type, Class, MemberPointer>(name);
    }
}
