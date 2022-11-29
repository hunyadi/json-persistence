#pragma once
#include "detail/defer.hpp"

/**
 * Provides compile-time access to a string literal.
 */
#define PERSISTENCE_MEMBER_NAME(string_literal) \
    []{ \
        struct descriptor { constexpr static const char* name() noexcept { return (string_literal); } }; \
        return descriptor(); \
    }()

/**
 * Helps enumerate member variables in a persistence template function.
 */
#define NAMED_MEMBER_VARIABLE(name, variable) \
    ::persistence::member_variable<&::persistence::remove_reference_t<decltype(*this)>::variable>(PERSISTENCE_MEMBER_NAME(name))

/**
 * Helps enumerate member variables in a persistence template function.
 */
#define MEMBER_VARIABLE(variable) \
    NAMED_MEMBER_VARIABLE(#variable, variable)

/**
 * Helps enumerate member variables in a persistence template function.
 */
#define NAMED_MEMBER_VARIABLE_DEFAULT(name, variable) \
    ::persistence::member_variable_default<&::persistence::remove_reference_t<decltype(*this)>::variable>(PERSISTENCE_MEMBER_NAME(name))

/**
 * Helps enumerate member variables in a persistence template function.
 */
#define MEMBER_VARIABLE_DEFAULT(variable) \
    NAMED_MEMBER_VARIABLE_DEFAULT(#variable, variable)

namespace persistence
{
    /** Provides the member type which is the type referred to by T. */
    template<typename T>
    struct remove_reference
    {
        using type = T;
    };

    template<typename T>
    struct remove_reference<T&>
    {
        using type = T;
    };

    template<typename T>
    using remove_reference_t = typename remove_reference<T>::type;

    namespace member
    {
        template<typename Type, class Class, auto Pointer, typename Descriptor>
        struct variable;

        template<typename Type, class Class, auto Pointer, typename Descriptor>
        struct variable_default;
    }

    template<typename Pointer>
    struct member_pointer_traits
    {
        static_assert(detail::fail<Pointer>, "expected a non-static member pointer");
    };

    template<class Class, typename Type>
    struct member_pointer_traits<Type Class::*>
    {
        using member_type = Type;
        using class_type = Class;
    };

    /**
     * Represents a member variable in a (de-)serialization annotation that has no default value.
     *
     * @tparam Pointer Pointer to the member being annotated.
     * @tparam Descriptor Structure that provides compile-time access to the member name as a string literal.
     */
    template<auto Pointer, typename Descriptor>
    constexpr auto member_variable(Descriptor)
    {
        using pointer_type = decltype(Pointer);    
        using traits = member_pointer_traits<pointer_type>;
        using Type = typename traits::member_type;
        using Class = typename traits::class_type;
        return member::variable<Type, Class, Pointer, Descriptor>();
    }

    /**
     * Represents a member variable in a (de-)serialization annotation that has a default value assigned
     * in a constructor or by a member initializer.
     *
     * @tparam Pointer Pointer to the member being annotated.
     * @tparam Descriptor Structure that provides compile-time access to the member name as a string literal.
     */
    template<auto Pointer, typename Descriptor>
    constexpr auto member_variable_default(Descriptor)
    {
        using pointer_type = decltype(Pointer);
        using traits = member_pointer_traits<pointer_type>;
        using Type = typename traits::member_type;
        using Class = typename traits::class_type;
        return member::variable_default<Type, Class, Pointer, Descriptor>();
    }
}
