#pragma once
#include "detail/version.hpp"
#include "detail/defer.hpp"
#include "detail/parse_event.hpp"
#include "detail/polymorphic_stack.hpp"
#include <string>

namespace persistence
{
    class ReaderContext
    {
    public:
        ReaderContext(JsonParseEventDispatcher& dispatcher)
            : dispatcher(dispatcher)
        {}

        ReaderContext(const ReaderContext&) = delete;

        template<typename C, typename... T>
        JsonParseEvent& emplace(T&&... args)
        {
            dispatcher.handler = &stack.emplace<C>(std::forward<T>(args)...);
            return *dispatcher.handler;
        }

        void pop()
        {
            stack.pop();
            dispatcher.handler = &stack.back();
        }

        template<typename C, typename... T>
        JsonParseEvent& replace(T&&... args)
        {
            stack.pop();
            return emplace<C>(std::forward<T>(args)...);
        }

        bool has_error() const
        {
            return !error_message.empty();
        }

        std::string get_error() const
        {
            return error_message;
        }

        void fail(std::string&& reason)
        {
            error_message = std::move(reason);
        }

        JsonParseEventDispatcher& dispatcher;

    private:
        detail::PolymorphicStack<JsonParseEvent> stack;
        std::string error_message;
    };

    struct JsonParseHandler : JsonParseEvent
    {
        JsonParseHandler(ReaderContext& context)
            : context(context)
        {}

        template<typename FoundToken>
        bool fail()
        {
            context.fail("unexpected JSON token " + std::string(FoundToken::name));
            return false;
        }

        bool parse(const JsonValueNull&) override
        {
            return fail<JsonValueNull>();
        }

        bool parse(const JsonValueBoolean&) override
        {
            return fail<JsonValueBoolean>();
        }

        bool parse(const JsonValueInteger&) override
        {
            return fail<JsonValueInteger>();
        }

        bool parse(const JsonValueUnsigned&) override
        {
            return fail<JsonValueUnsigned>();
        }

        bool parse(const JsonValueInteger64&) override
        {
            return fail<JsonValueInteger64>();
        }

        bool parse(const JsonValueUnsigned64&) override
        {
            return fail<JsonValueUnsigned64>();
        }

        bool parse(const JsonValueDouble&) override
        {
            return fail<JsonValueDouble>();
        }

        bool parse(const JsonValueNumber&) override
        {
            return fail<JsonValueNumber>();
        }

        bool parse(const JsonValueString&) override
        {
            return fail<JsonValueString>();
        }

        bool parse(const JsonObjectStart&) override
        {
            return fail<JsonObjectStart>();
        }

        bool parse(const JsonObjectKey&) override
        {
            return fail<JsonObjectKey>();
        }

        bool parse(const JsonObjectEnd&) override
        {
            return fail<JsonObjectEnd>();
        }

        bool parse(const JsonArrayStart&) override
        {
            return fail<JsonArrayStart>();
        }

        bool parse(const JsonArrayEnd&) override
        {
            return fail<JsonArrayEnd>();
        }

    protected:
        ReaderContext& context;
    };

    template<typename ExpectToken>
    struct JsonParseSingleHandler : JsonParseEvent
    {
        JsonParseSingleHandler(ReaderContext& context)
            : context(context)
        {}

        template<typename FoundToken>
        bool fail()
        {
            context.fail("expected JSON token " + std::string(ExpectToken::name) + "; got: " + std::string(FoundToken::name));
            return false;
        }

        bool parse(const JsonValueNull&) override
        {
            return fail<JsonValueNull>();
        }

        bool parse(const JsonValueBoolean&) override
        {
            return fail<JsonValueBoolean>();
        }

        bool parse(const JsonValueInteger&) override
        {
            return fail<JsonValueInteger>();
        }

        bool parse(const JsonValueUnsigned&) override
        {
            return fail<JsonValueUnsigned>();
        }

        bool parse(const JsonValueInteger64&) override
        {
            return fail<JsonValueInteger64>();
        }

        bool parse(const JsonValueUnsigned64&) override
        {
            return fail<JsonValueUnsigned64>();
        }

        bool parse(const JsonValueDouble&) override
        {
            return fail<JsonValueDouble>();
        }

        bool parse(const JsonValueNumber&) override
        {
            return fail<JsonValueNumber>();
        }

        bool parse(const JsonValueString&) override
        {
            return fail<JsonValueString>();
        }

        bool parse(const JsonObjectStart&) override
        {
            return fail<JsonObjectStart>();
        }

        bool parse(const JsonObjectKey&) override
        {
            return fail<JsonObjectKey>();
        }

        bool parse(const JsonObjectEnd&) override
        {
            return fail<JsonObjectEnd>();
        }

        bool parse(const JsonArrayStart&) override
        {
            return fail<JsonArrayStart>();
        }

        bool parse(const JsonArrayEnd&) override
        {
            return fail<JsonArrayEnd>();
        }

    protected:
        ReaderContext& context;
    };

    template<typename T, typename Enable = void>
    struct JsonParser
    {
        // if you are getting a compile-time error pointing at this location, make sure the appropriate
        // headers are included, and (de-)serialization is supported for the type
        static_assert(detail::fail<T>, "expected a type that can be deserialized from JSON");
    };
}
