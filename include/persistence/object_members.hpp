#pragma once
#include "object.hpp"
#include <tuple>

namespace persistence
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
    };

    template<typename Class>
    constexpr auto member_variables(Class& cls)
    {
        constexpr ObjectMemberBuilder<Class, std::tuple<>> builder;
        return cls.persist(builder).members;
    }
}
