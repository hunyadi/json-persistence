#pragma once
#include <string_view>

namespace persistence
{
    struct JsonValueNull
    {
        constexpr static const char* name = "null";
    };

    struct JsonValueBoolean
    {
        constexpr static const char* name = "boolean";

        bool value;

        JsonValueBoolean(bool value) : value(value) {}
    };

    struct JsonValueInteger
    {
        constexpr static const char* name = "number";

        int value;

        JsonValueInteger(int value) : value(value) {}
    };

    struct JsonValueUnsigned
    {
        constexpr static const char* name = "number";

        unsigned int value;

        JsonValueUnsigned(unsigned int value) : value(value) {}
    };

    struct JsonValueInteger64
    {
        constexpr static const char* name = "number";

        std::int64_t value;

        JsonValueInteger64(std::int64_t value) : value(value) {}
    };

    struct JsonValueUnsigned64
    {
        constexpr static const char* name = "number";

        std::uint64_t value;

        JsonValueUnsigned64(std::uint64_t value) : value(value) {}
    };

    struct JsonValueDouble
    {
        constexpr static const char* name = "number";

        double value;

        JsonValueDouble(double value) : value(value) {}
    };

    struct JsonValueNumber
    {
        constexpr static const char* name = "number";

        std::string_view literal;

        JsonValueNumber() = default;
        JsonValueNumber(const char* str, std::size_t len) : literal(str, len) {}
    };

    struct JsonValueString
    {
        constexpr static const char* name = "string";

        std::string_view literal;

        JsonValueString() = default;
        JsonValueString(const char* str, std::size_t len) : literal(str, len) {}
    };

    struct JsonObjectStart
    {
        constexpr static const char* name = "'{'";
    };

    struct JsonObjectKey
    {
        constexpr static const char* name = "key name";

        std::string_view identifier;

        JsonObjectKey() = default;
        JsonObjectKey(const char* str, std::size_t len) : identifier(str, len) {}
    };

    struct JsonObjectEnd
    {
        constexpr static const char* name = "'}'";
    };

    struct JsonArrayStart
    {
        constexpr static const char* name = "'['";
    };

    struct JsonArrayEnd
    {
        constexpr static const char* name = "']'";
    };

    struct JsonParseEvent
    {
        virtual ~JsonParseEvent() {}
        virtual bool parse(const JsonValueNull&) = 0;
        virtual bool parse(const JsonValueBoolean&) = 0;
        virtual bool parse(const JsonValueInteger&) = 0;
        virtual bool parse(const JsonValueUnsigned&) = 0;
        virtual bool parse(const JsonValueInteger64&) = 0;
        virtual bool parse(const JsonValueUnsigned64&) = 0;
        virtual bool parse(const JsonValueDouble&) = 0;
        virtual bool parse(const JsonValueNumber&) = 0;
        virtual bool parse(const JsonValueString&) = 0;
        virtual bool parse(const JsonObjectStart&) = 0;
        virtual bool parse(const JsonObjectKey&) = 0;
        virtual bool parse(const JsonObjectEnd&) = 0;
        virtual bool parse(const JsonArrayStart&) = 0;
        virtual bool parse(const JsonArrayEnd&) = 0;
    };

    struct JsonParseEventDispatcher
    {
        bool Int(int value)
        {
            return handler->parse(JsonValueInteger(value));
        }

        bool Uint(unsigned int value)
        {
            return handler->parse(JsonValueUnsigned(value));
        }

        bool Int64(std::int64_t value)
        {
            return handler->parse(JsonValueInteger64(value));
        }

        bool Uint64(std::uint64_t value)
        {
            return handler->parse(JsonValueUnsigned64(value));
        }

        bool Double(double value)
        {
            return handler->parse(JsonValueDouble(value));
        }

        bool Null()
        {
            return handler->parse(JsonValueNull());
        }

        bool Bool(bool value)
        {
            return handler->parse(JsonValueBoolean(value));
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

        JsonParseEvent* handler = nullptr;
    };
}
