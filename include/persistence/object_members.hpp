#pragma once
#include "object.hpp"
#include <tuple>

namespace persistence
{
    namespace detail
    {
        /**
        * Enumerates member variables of a class at compile-time.
        */
        template<typename Class, typename MemberList>
        struct ObjectMemberBuilder
        {
            constexpr ObjectMemberBuilder(const MemberList& members)
                : members(members)
            {}

            template<typename T, typename Base>
            constexpr auto operator&(const member_variable<T, Base>& member) const
            {
                static_assert(std::is_base_of_v<Base, Class>, "expected a member variable part of the class inheritance chain");
                auto tpl = std::tuple_cat(members, std::make_tuple(member));
                return ObjectMemberBuilder<Class, decltype(tpl)>(tpl);
            }

            MemberList members;
        };

        template<typename Class>
        struct ObjectMemberBuilder<Class, std::tuple<>>
        {
            constexpr ObjectMemberBuilder() = default;

            template<typename T, typename Base>
            constexpr auto operator&(const member_variable<T, Base>& member) const
            {
                static_assert(std::is_base_of_v<Base, Class>, "expected a member variable part of the class inheritance chain");
                auto tpl = std::make_tuple(member);
                return ObjectMemberBuilder<Class, decltype(tpl)>(tpl);
            }

            std::tuple<> members;
        };
    }

    template<typename Class, std::size_t Count = 0>
    struct ObjectMemberCounter
    {
        constexpr static std::size_t count = Count;

        constexpr ObjectMemberCounter() = default;

        template<typename T, typename Base>
        constexpr auto operator&(const member_variable<T, Base>&) const
        {
            static_assert(std::is_base_of_v<Base, Class>, "expected a member variable part of the class inheritance chain");
            return ObjectMemberCounter<Class, Count + 1>();
        }
    };

    template<typename Class>
    constexpr auto member_variables(const Class& cls)
    {
        static_assert(std::is_class_v<Class>, "expected a class type");
        constexpr auto builder = detail::ObjectMemberBuilder<Class, std::tuple<>>();
        return const_cast<Class&>(cls).persist(builder).members;
    }
}
