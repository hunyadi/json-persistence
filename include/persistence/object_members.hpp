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

            template<typename T>
            constexpr auto operator&(const member_variable<T, Class>& member) const
            {
                auto tpl = std::tuple_cat(members, std::make_tuple(member));
                return ObjectMemberBuilder<Class, decltype(tpl)>(tpl);
            }

            MemberList members;
        };

        template<typename Class>
        struct ObjectMemberBuilder<Class, std::tuple<>>
        {
            constexpr ObjectMemberBuilder() = default;

            template<typename T>
            constexpr auto operator&(const member_variable<T, Class>& member) const
            {
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

        template<typename T>
        constexpr auto operator&(const member_variable<T, Class>&) const
        {
            return ObjectMemberCounter<Class, Count + 1>();
        }
    };

    template<typename Class>
    constexpr auto member_variables(Class& cls)
    {
        static_assert(std::is_class_v<Class>, "expected a class type");
        constexpr auto builder = detail::ObjectMemberBuilder<Class, std::tuple<>>();
        return cls.persist(builder).members;
    }
}
