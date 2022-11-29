#pragma once
#include "object.hpp"
#include <tuple>

namespace persistence
{
    /**
     * Enumerates member variables of a class at compile-time.
     *
     * @tparam Class A class with the member function `persist`.
     * @tparam Members A list of persisted member variables that the class has.
     */
    template<typename Class, typename... Members>
    struct ObjectMemberBuilder
    {
        using member_types = std::tuple<Members...>;

        constexpr ObjectMemberBuilder() = default;

        template<typename T, class B, auto P, typename D>
        constexpr auto operator&(const member::variable<T, B, P, D>&) const
        {
            static_assert(std::is_base_of_v<B, Class>, "expected a member variable part of the class inheritance chain");
            return ObjectMemberBuilder<Class, Members..., member::variable<T, B, P, D>>();
        }

        template<typename T, class B, auto P, typename D>
        constexpr auto operator&(const member::variable_default<T, B, P, D>&) const
        {
            static_assert(std::is_base_of_v<B, Class>, "expected a member variable part of the class inheritance chain");
            return ObjectMemberBuilder<Class, Members..., member::variable_default<T, B, P, D>>();
        }
    };

    /**
     * Counts the member variables of a class at compile-time.
     *
     * @tparam Class A class with the member function `persist`.
     * @tparam Count The number of persisted member variables that the class has.
     */
    template<typename Class, std::size_t Count = 0>
    struct ObjectMemberCounter
    {
        constexpr static std::size_t count = Count;

        constexpr ObjectMemberCounter() = default;

        template<typename T, class B, auto P, typename D>
        constexpr auto operator&(const member::variable<T, B, P, D>&) const
        {
            return ObjectMemberCounter<Class, Count + 1>();
        }

        template<typename T, class B, auto P, typename D>
        constexpr auto operator&(const member::variable_default<T, B, P, D>&) const
        {
            return ObjectMemberCounter<Class, Count + 1>();
        }
    };

    /**
     * Gives information about a class type that supports persistence.
     *
     * @tparam Class A class with the member function `persist`.
     */
    template<class Class>
    struct class_traits
    {
        static_assert(std::is_class_v<Class>, "expected a class type");
        using member_types = typename decltype(
            std::declval<Class&>().persist(std::declval<ObjectMemberBuilder<Class>&>())
        )::member_types;
    };
}
