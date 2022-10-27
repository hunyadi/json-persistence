#pragma once
#include "object.hpp"
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
    struct JsonFieldFactory
    {
        virtual ~JsonFieldFactory() {}
        virtual void create(ReaderContext& context, C& ref) = 0;
    };

    template<typename T, typename C>
    struct JsonTypedFieldFactory : JsonFieldFactory<C>
    {
        JsonTypedFieldFactory(const member_variable<T, C>& member)
            : member(member)
        {}

        virtual void create(ReaderContext& context, C& ref)
        {
            context.emplace<JsonParser<T>>(context, member.ref(ref));
        }

    private:
        member_variable<T, C> member;
    };

    template<typename C>
    using field_factory_map = std::vector<std::pair<std::string_view, std::unique_ptr<JsonFieldFactory<C>>>>;

    template<typename C>
    struct ObjectMemberVisitor
    {
        ObjectMemberVisitor(field_factory_map<C>& items)
            : items(items)
        {}

        template<typename T>
        ObjectMemberVisitor& operator&(const member_variable<T, C>& member)
        {
            std::unique_ptr<JsonFieldFactory<C>> factory = std::make_unique<JsonTypedFieldFactory<T, C>>(member);
            items.push_back(std::make_pair(member.name, std::move(factory)));
            return *this;
        }

    private:
        field_factory_map<C>& items;
    };

    template<typename C>
    struct JsonObjectParser : EventHandler
    {
        static_assert(std::is_class<C>::value, "expected a class type");

    private:
        static field_factory_map<C> initialize()
        {
            field_factory_map<C> items;
            ObjectMemberVisitor<C> visitor(items);
            C().persist(visitor);
            return items;
        }

    public:
        static inline field_factory_map<C> parsers = initialize();

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
            for (auto&& p : parsers) {
                if (p.first == key) {
                    p.second->create(context, ref);
                    return true;
                }
            }
            return false;
        }

    private:
        ReaderContext& context;
        C& ref;
    };

    template<typename, typename = void>
    struct has_custom_parse : std::false_type {};

    template<typename T>
    struct has_custom_parse<T, std::void_t<
        decltype(
            std::declval<T&>().persist(std::declval<ObjectMemberVisitor<T>&>())
        )
    >> : std::true_type {};

    template<typename T>
    struct JsonParser<T, typename std::enable_if<has_custom_parse<T>::value>::type> : EventHandler
    {
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
