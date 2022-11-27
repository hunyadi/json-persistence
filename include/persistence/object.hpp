#pragma once
#include <string_view>
#include <type_traits>

/**
 * Provides compile-time access to a string literal.
 */
#define PERSISTENCE_MEMBER_NAME(string_literal) \
    []{ \
        struct descriptor { constexpr static std::string_view name() noexcept { return (string_literal); } }; \
        return descriptor(); \
    }()

/**
 * Helps enumerate member variables in a persistence template function.
 */
#define NAMED_MEMBER_VARIABLE(name, variable) \
    ::persistence::class_member_variable<&std::remove_reference_t<decltype(*this)>::variable>(PERSISTENCE_MEMBER_NAME(name))

/**
 * Helps enumerate member variables in a persistence template function.
 */
#define MEMBER_VARIABLE(variable) \
    NAMED_MEMBER_VARIABLE(#variable, variable)

/**
 * Helps enumerate member variables in a persistence template function.
 */
#define NAMED_MEMBER_VARIABLE_DEFAULT(name, variable) \
    ::persistence::class_member_variable_default<&std::remove_reference_t<decltype(*this)>::variable>(PERSISTENCE_MEMBER_NAME(name))

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
     *
     * @tparam Type The type of the member variable bound by this structure.
     * @tparam Class The type of the encapsulating object.
     * @tparam Pointer The member pointer value.
     */
    template<typename Type, class Class, Type Class::* Pointer, typename Descriptor>
    struct member_variable_declaration
    {
        using pointer_type = decltype(Pointer);
        /** The type of the member variable bound by this structure. */
        using member_type = Type;
        /** The type of the encapsulating object. */
        using class_type = Class;

        /**
         * The name of the member variable as a constant string literal.
         */
        constexpr std::string_view name() const
        {
            return Descriptor::name();
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

    protected:
        constexpr member_variable_declaration() = default;
    };

    /**
     * Represents a member variable in a (de-)serialization declaration with no default value.
     *
     * @tparam Type The type of the member variable bound by this structure.
     * @tparam Class The type of the encapsulating object.
     * @tparam Pointer The member pointer value.
     */
    template<typename Type, class Class, auto Pointer, typename Descriptor>
    struct member_variable final : member_variable_declaration<Type, Class, Pointer, Descriptor>
    {
        using member_variable_declaration<Type, Class, Pointer, Descriptor>::member_variable_declaration;
    };

    /**
     * Represents a member variable in a (de-)serialization declaration with default value assigned
     * using a member initializer.
     *
     * @tparam Type The type of the member variable bound by this structure.
     * @tparam Class The type of the encapsulating object.
     * @tparam Pointer The member pointer value.
     */
    template<typename Type, class Class, auto Pointer, typename Descriptor>
    struct member_variable_default final : member_variable_declaration<Type, Class, Pointer, Descriptor>
    {
        using member_variable_declaration<Type, Class, Pointer, Descriptor>::member_variable_declaration;
    };

    template<auto MemberPointer, typename Descriptor>
    constexpr auto class_member_variable(Descriptor)
    {
        using pointer_type = decltype(MemberPointer);
        static_assert(std::is_member_object_pointer_v<pointer_type>, "expected a non-static member object pointer");
        using traits = member_pointer_traits<pointer_type>;
        using Type = typename traits::member_type;
        using Class = typename traits::class_type;
        return member_variable<Type, Class, MemberPointer, Descriptor>();
    }

    template<auto MemberPointer, typename Descriptor>
    constexpr auto class_member_variable_default(Descriptor)
    {
        using pointer_type = decltype(MemberPointer);
        static_assert(std::is_member_object_pointer_v<pointer_type>, "expected a non-static member object pointer");
        using traits = member_pointer_traits<pointer_type>;
        using Type = typename traits::member_type;
        using Class = typename traits::class_type;
        return member_variable_default<Type, Class, MemberPointer, Descriptor>();
    }
}
