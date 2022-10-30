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
        static_assert(std::is_class<C>::value, "expected a class type");

    public:
        JsonObjectParser(ReaderContext& context, C& ref)
            : context(context)
            , ref(ref)
        {}

        bool parse(JsonObjectEnd) override
        {
            context.pop();
            return true;
        }

        bool parse(JsonObjectKey json_key) override
        {
            std::string_view key = json_key.identifier;
            return std::apply([=](const auto&... members) {
                return (parse_member(key, members) || ...);
            }, members);
        }

    private:
        static auto initialize()
        {
            C cls;
            return member_variables(cls);
        }

        template<typename T>
        bool parse_member(const std::string_view& key, const member_variable<T, C>& member)
        {
            if (member.name == key) {
                context.emplace<JsonParser<T>>(context, member.ref(ref));
                return true;
            } else {
                return false;
            }
        }

    private:
        inline static auto members = initialize();
        ReaderContext& context;
        C& ref;
    };

    template<typename T>
    using parser_function = decltype(std::declval<T&>().persist(std::declval<ObjectMemberCounter<T>&>()));

    template<typename T, typename Enable = void>
    struct has_custom_parser : std::false_type
    {};

    template<typename T>
    struct has_custom_parser<T, typename std::enable_if<std::is_class<T>::value>::type>
    {
        constexpr static bool value = detect<T, parser_function>::value;
    };

    template<typename T>
    struct JsonParser<T, typename std::enable_if<has_custom_parser<T>::value>::type> : EventHandler
    {
        static_assert(!std::is_same<parser_function<T>, void>::value, "`persist` function cannot have a return type of `void`, use `auto` instead");

        using json_type = JsonObjectStart;

        JsonParser(ReaderContext& context, T& ref)
            : context(context)
            , ref(ref)
        {}

        bool parse(JsonObjectStart) override
        {
            context.replace<JsonObjectParser<T>>(context, ref);
            return true;
        }

    private:
        ReaderContext& context;
        T& ref;
    };
}
