#pragma once
#include "object.hpp"
#include "detail/make_array.hpp"
#include <tuple>

namespace persistence
{
    /**
    * Enumerates member variables of a class at compile-time.
    */
    template<typename Class, typename... Members>
    struct ObjectMemberBuilder
    {
        using member_types = std::tuple<Members...>;

        constexpr ObjectMemberBuilder(Members... members)
            : members(member_types(members...))
        {}

        template<typename T, class B, auto P>
        constexpr auto operator&(const member_variable<T, B, P>& member) const
        {
            static_assert(std::is_base_of_v<B, Class>, "expected a member variable part of the class inheritance chain");
            return std::apply([=](auto&&... args) {
                return ObjectMemberBuilder<Class, Members..., member_variable<T, B, P>>(args..., member);
            }, members);
        }

        template<typename T, class B, auto P>
        constexpr auto operator&(const member_variable_default<T, B, P>& member) const
        {
            static_assert(std::is_base_of_v<B, Class>, "expected a member variable part of the class inheritance chain");
            return std::apply([=](auto&&... args) {
                return ObjectMemberBuilder<Class, Members..., member_variable_default<T, B, P>>(args..., member);
            }, members);
        }

        member_types members;
    };

    template<auto... Ts>
    struct item_list
    {};

    /**
    * Enumerates member variable pointers of a class at compile-time.
    */
    template<typename Class, auto... Pointers>
    struct ObjectPointerBuilder
    {
        using pointers = item_list<Pointers...>;

        template<typename T, class B, auto P>
        constexpr auto operator&(const member_variable<T, B, P>&) const
        {
            return ObjectPointerBuilder<Class, Pointers..., P>();
        }

        template<typename T, class B, auto P>
        constexpr auto operator&(const member_variable_default<T, B, P>&) const
        {
            return ObjectPointerBuilder<Class, Pointers..., P>();
        }
    };

    template<typename Class, std::size_t Count = 0>
    struct ObjectMemberCounter
    {
        constexpr static std::size_t count = Count;

        constexpr ObjectMemberCounter() = default;

        template<typename T, class B, auto P>
        constexpr auto operator&(const member_variable<T, B, P>&) const
        {
            return ObjectMemberCounter<Class, Count + 1>();
        }

        template<typename T, class B, auto P>
        constexpr auto operator&(const member_variable_default<T, B, P>&) const
        {
            return ObjectMemberCounter<Class, Count + 1>();
        }
    };

    template<typename Class>
    struct object_member_traits
    {
        using member_types = typename decltype(
            std::declval<Class&>().persist(std::declval<ObjectMemberBuilder<Class>&>())
        )::member_types;

        constexpr inline static auto pointers = decltype(
            std::declval<Class&>().persist(std::declval<ObjectPointerBuilder<Class>&>())
        )::pointers;
    };

    template<typename Class>
    constexpr auto member_variables(const Class& cls)
    {
        static_assert(std::is_class_v<Class>, "expected a class type");
        constexpr auto builder = ObjectMemberBuilder<Class>();
        auto result = const_cast<Class&>(cls).persist(builder);
        return result.members;
    }

    template<typename Class>
    constexpr auto member_names(const Class& cls)
    {
        static_assert(std::is_class_v<Class>, "expected a class type");
        constexpr auto builder = ObjectMemberBuilder<Class>();
        auto result = const_cast<Class&>(cls).persist(builder);
        return std::apply([=](auto&&... args) {
            return make_array(args.name()...);
        }, result.members);
    }
}
