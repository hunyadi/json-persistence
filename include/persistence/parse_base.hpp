#pragma once
#include "detail/version.hpp"
#include "detail/defer.hpp"
#include "detail/parse_event.hpp"
#include "detail/polymorphic_stack.hpp"
#include <string>

namespace persistence
{
    struct EventDispatcher
    {
        bool Int(int) { return false; }
        bool Uint(unsigned int) { return false; }
        bool Int64(int64_t) { return false; }
        bool Uint64(uint64_t) { return false; }
        bool Double(double) { return false; }

        bool Null()
        {
            return handler->parse(JsonValueNull());
        }

        bool Bool(bool b)
        {
            return handler->parse(JsonValueBoolean(b));
        }

        bool RawNumber(const char* str, std::size_t length, bool /*copy*/)
        {
            return handler->parse(JsonValueNumber(str, length));
        }

        bool String(const char* str, std::size_t length, bool /*copy*/)
        {
            return handler->parse(JsonValueString(str, length));
        }

        bool StartObject()
        {
            return handler->parse(JsonObjectStart());
        }

        bool Key(const char* str, std::size_t length, bool /*copy*/)
        {
            return handler->parse(JsonObjectKey(str, length));
        }

        bool EndObject(std::size_t /*memberCount*/)
        {
            return handler->parse(JsonObjectEnd());
        }

        bool StartArray()
        {
            return handler->parse(JsonArrayStart());
        }

        bool EndArray(std::size_t /*elementCount*/)
        {
            return handler->parse(JsonArrayEnd());
        }

        EventHandler* handler = nullptr;
    };

    class ReaderContext
    {
    public:
        ReaderContext(EventDispatcher& dispatcher)
            : dispatcher(dispatcher)
        {}

        ReaderContext(const ReaderContext&) = delete;

        template<typename C, typename... T>
        EventHandler& emplace(T&&... args)
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
        EventHandler& replace(T&&... args)
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

        EventDispatcher& dispatcher;

    private:
        detail::PolymorphicStack<EventHandler> stack;
        std::string error_message;
    };

    struct JsonEventHandler : EventHandler
    {
        JsonEventHandler(ReaderContext& context)
            : context(context)
        {}

        template<typename FoundToken>
        bool fail()
        {
            context.fail("unexpected JSON token " + std::string(FoundToken::name));
            return false;
        }

        virtual bool parse(const JsonValueNull&)
        {
            return fail<JsonValueNull>();
        }

        virtual bool parse(const JsonValueBoolean&)
        {
            return fail<JsonValueBoolean>();
        }

        virtual bool parse(const JsonValueNumber&)
        {
            return fail<JsonValueNumber>();
        }

        virtual bool parse(const JsonValueString&)
        {
            return fail<JsonValueString>();
        }

        virtual bool parse(const JsonObjectStart&)
        {
            return fail<JsonObjectStart>();
        }

        virtual bool parse(const JsonObjectKey&)
        {
            return fail<JsonObjectKey>();
        }

        virtual bool parse(const JsonObjectEnd&)
        {
            return fail<JsonObjectEnd>();
        }

        virtual bool parse(const JsonArrayStart&)
        {
            return fail<JsonArrayStart>();
        }

        virtual bool parse(const JsonArrayEnd&)
        {
            return fail<JsonArrayEnd>();
        }

    protected:
        ReaderContext& context;
    };

    template<typename ExpectToken>
    struct JsonSingleEventHandler : EventHandler
    {
        JsonSingleEventHandler(ReaderContext& context)
            : context(context)
        {}

        template<typename FoundToken>
        bool fail()
        {
            context.fail("expected JSON token " + std::string(ExpectToken::name) + "; got: " + std::string(FoundToken::name));
            return false;
        }

        virtual bool parse(const JsonValueNull&)
        {
            return fail<JsonValueNull>();
        }

        virtual bool parse(const JsonValueBoolean&)
        {
            return fail<JsonValueBoolean>();
        }

        virtual bool parse(const JsonValueNumber&)
        {
            return fail<JsonValueNumber>();
        }

        virtual bool parse(const JsonValueString&)
        {
            return fail<JsonValueString>();
        }

        virtual bool parse(const JsonObjectStart&)
        {
            return fail<JsonObjectStart>();
        }

        virtual bool parse(const JsonObjectKey&)
        {
            return fail<JsonObjectKey>();
        }

        virtual bool parse(const JsonObjectEnd&)
        {
            return fail<JsonObjectEnd>();
        }

        virtual bool parse(const JsonArrayStart&)
        {
            return fail<JsonArrayStart>();
        }

        virtual bool parse(const JsonArrayEnd&)
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
