#pragma once
#include "detail/version.hpp"
#include "detail/defer.hpp"
#include "detail/polymorphic_stack.hpp"
#include <memory>
#include <string_view>
#include <vector>

namespace persistence
{
    struct JsonValueNull
    {};

    struct JsonValueBoolean
    {
        bool value;

        JsonValueBoolean(bool value = false) : value(value) {}
    };

    struct JsonValueNumber
    {
        std::string_view literal;

        JsonValueNumber() = default;
        JsonValueNumber(const char* str, std::size_t len) : literal(str, len) {}
    };

    struct JsonValueString
    {
        std::string_view literal;

        JsonValueString() = default;
        JsonValueString(const char* str, std::size_t len) : literal(str, len) {}
    };

    struct JsonObjectStart
    {};

    struct JsonObjectKey
    {
        std::string_view identifier;

        JsonObjectKey() = default;
        JsonObjectKey(const char* str, std::size_t len) : identifier(str, len) {}
    };

    struct JsonObjectEnd
    {};

    struct JsonArrayStart
    {};

    struct JsonArrayEnd
    {};

    struct EventHandler
    {
        virtual ~EventHandler() {}
        virtual bool parse(JsonValueNull) { return false; }
        virtual bool parse(JsonValueBoolean) { return false; }
        virtual bool parse(JsonValueNumber) { return false; }
        virtual bool parse(JsonValueString) { return false; }
        virtual bool parse(JsonObjectStart) { return false; }
        virtual bool parse(JsonObjectKey) { return false; }
        virtual bool parse(JsonObjectEnd) { return false; }
        virtual bool parse(JsonArrayStart) { return false; }
        virtual bool parse(JsonArrayEnd) { return false; }
    };

    struct EventDispatcher
    {
        EventHandler* handler = nullptr;

        bool Null() { return handler->parse(JsonValueNull()); }
        bool Bool(bool b) { return handler->parse(JsonValueBoolean(b)); }
        bool Int(int) { return false; }
        bool Uint(unsigned) { return false; }
        bool Int64(int64_t) { return false; }
        bool Uint64(uint64_t) { return false; }
        bool Double(double) { return false; }

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

    private:
        EventDispatcher& dispatcher;
        detail::PolymorphicStack<EventHandler> stack;
    };

    template<typename T, typename Enable = void>
    struct JsonParser
    {
        // if you are getting a compile-time error pointing at this location, make sure the appropriate
        // headers are included, and (de-)serialization is supported for the type
        static_assert(detail::fail<T>, "expected a type that can be deserialized from JSON");
    };
}
