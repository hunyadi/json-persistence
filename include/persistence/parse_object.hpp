#pragma once
#include "object.hpp"
#include "object_members.hpp"
#include "parse_base.hpp"
#include <optional>

namespace persistence
{
    template<typename T>
    struct required_type {
        using type = T;
    };

    template<typename T>
    struct required_type<std::optional<T>> {
        using type = T;
    };

    template<typename T>
    using required_type_t = typename required_type<T>::type;

    template<typename T>
    struct JsonParser<std::optional<T>> : JsonParser<T>
    {
        JsonParser(ReaderContext& context, std::optional<T>& ref)
            : JsonParser<T>(context, ref.emplace())
        {}
    };

    template<typename C>
    struct JsonObjectParser : EventHandler
    {
        static_assert(std::is_class_v<C>, "expected a class type");

    public:
        JsonObjectParser(ReaderContext& context, C& ref)
            : context(context)
            , ref(ref)
        {}

        bool parse(const JsonObjectEnd&) override
        {
            context.pop();
            return true;
        }

        bool parse(const JsonObjectKey& json_key) override
        {
            std::string_view key = json_key.identifier;
            return std::apply([=](const auto&... members) {
                return (parse_member(key, members) || ...);
            }, members);
        }

    private:
        template<typename T, typename B>
        bool parse_member(const std::string_view& key, const member_variable<T, B>& member)
        {
            static_assert(std::is_base_of_v<B, C>, "expected a member variable part of the class inheritance chain");
            if (member.name() == key) {
                context.emplace<JsonParser<T>>(context, member.ref(ref));
                return true;
            } else {
                return false;
            }
        }

    private:
        inline static auto members = member_variables(C());
        ReaderContext& context;
        C& ref;
    };

    template<typename T>
    using parser_function = decltype(std::declval<T&>().persist(std::declval<ObjectMemberCounter<T>&>()));

    template<typename T, typename Enable = void>
    struct has_custom_parser : std::false_type
    {};

    template<typename T>
    struct has_custom_parser<T, std::enable_if_t<std::is_class_v<T>>>
    {
        constexpr static bool value = detect<T, parser_function>::value;
    };

    template<typename T>
    struct JsonParser<T, std::enable_if_t<has_custom_parser<T>::value>> : EventHandler
    {
        static_assert(!std::is_same_v<parser_function<T>, void>, "`persist` function cannot have a return type of `void`, use `auto` instead");

        using json_type = JsonObjectStart;

        JsonParser(ReaderContext& context, T& ref)
            : context(context)
            , ref(ref)
        {}

        bool parse(const JsonObjectStart&) override
        {
            context.replace<JsonObjectParser<T>>(context, ref);
            return true;
        }

    private:
        ReaderContext& context;
        T& ref;
    };
}
