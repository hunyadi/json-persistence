#pragma once
#include "object.hpp"
#include "object_members.hpp"
#include "parse_base.hpp"
#include "detail/make_array.hpp"
#include "detail/perfect_hash.hpp"
#include "detail/traits.hpp"
#include "detail/unlikely.hpp"
#include <optional>

namespace persistence
{
    namespace detail
    {
        template<std::size_t S, std::size_t I>
        struct visit_impl
        {
            template<typename T, typename F>
            static void visit(T& tup, std::size_t idx, F fun)
            {
                if (idx == S - I) {
                    fun(std::get<S - I>(tup));
                } else {
                    visit_impl<S, I - 1>::visit(tup, idx, fun);
                }
            }
        };

        template<std::size_t S>
        struct visit_impl<S, 0>
        {
            template<typename T, typename F>
            static void visit(T&, std::size_t, F)
            {}
        };
    }

    /**
     * Evaluates a function on the n-th element of a tuple.
     *
     * @param tup The tuple on whose element to evaluate the function.
     * @param idx The zero-based index of the tuple element on which to invoke the function.
     * @param fun The function to evaluate.
     */
    template<typename F, typename... Ts>
    void visit_at(const std::tuple<Ts...>& tup, std::size_t idx, F fun)
    {
        detail::visit_impl<sizeof...(Ts), sizeof...(Ts)>::visit(tup, idx, fun);
    }

    /**
     * Evaluates a function on the n-th element of a tuple.
     *
     * @param tup The tuple on whose element to evaluate the function.
     * @param idx The zero-based index of the tuple element on which to invoke the function.
     * @param fun The function to evaluate.
     */
    template<typename F, typename... Ts>
    void visit_at(std::tuple<Ts...>& tup, std::size_t idx, F fun)
    {
        detail::visit_impl<sizeof...(Ts), sizeof...(Ts)>::visit(tup, idx, fun);
    }

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
    struct JsonObjectParser : JsonParseHandler<JsonObjectKey, JsonObjectEnd>
    {
        static_assert(std::is_class_v<C>, "expected a class type");

    public:
        JsonObjectParser(ReaderContext& context, C& ref)
            : JsonParseHandler(context)
            , ref(ref)
        {}

        bool parse(const JsonObjectEnd&) override
        {
            context.pop();
            return true;
        }

        bool parse(const JsonObjectKey& json_key) override
        {
            std::string_view identifier = json_key.identifier;
            std::size_t k = hash_map.index(identifier);
            PERSISTENCE_IF_UNLIKELY(names[k] != identifier) {
                context.fail("expected class member name; got: " + std::string(json_key.identifier));
                return false;
            }

            visit_at(members, k, [&](auto&& member) {
                using parser_type = JsonParser<unqualified_t<decltype(member.ref(ref))>>;
                context.emplace<parser_type>(context, member.ref(ref));
            });
            return true;
        }

    private:
        constexpr static auto member_names()
        {
            return std::apply([](auto&&... args) {
                return make_array(args.name()...);
            }, typename class_traits<C>::member_types());
        }

        constexpr static auto members = typename class_traits<C>::member_types();
        inline static auto names = member_names();
        inline static auto hash_map = PerfectHash(names);
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
    struct JsonParser<T, std::enable_if_t<has_custom_parser<T>::value>> : JsonParseHandler<JsonObjectStart>
    {
        static_assert(!std::is_same_v<parser_function<T>, void>, "`persist` function cannot have a return type of `void`, use `auto` instead");

        using json_type = JsonObjectStart;

        JsonParser(ReaderContext& context, T& ref)
            : JsonParseHandler(context)
            , ref(ref)
        {}

        bool parse(const JsonObjectStart&) override
        {
            context.replace<JsonObjectParser<T>>(context, ref);
            return true;
        }

    private:
        T& ref;
    };
}
