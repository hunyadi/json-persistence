#pragma once
#include <string_view>

namespace persistence
{
    struct JsonValueNull
    {
        static constexpr const char* name = "null";
    };

    struct JsonValueBoolean
    {
        static constexpr const char* name = "boolean";

        bool value;

        JsonValueBoolean(bool value = false) : value(value) {}
    };

    struct JsonValueNumber
    {
        static constexpr const char* name = "number";

        std::string_view literal;

        JsonValueNumber() = default;
        JsonValueNumber(const char* str, std::size_t len) : literal(str, len) {}
    };

    struct JsonValueString
    {
        static constexpr const char* name = "string";

        std::string_view literal;

        JsonValueString() = default;
        JsonValueString(const char* str, std::size_t len) : literal(str, len) {}
    };

    struct JsonObjectStart
    {
        static constexpr const char* name = "'{'";
    };

    struct JsonObjectKey
    {
        static constexpr const char* name = "key name";

        std::string_view identifier;

        JsonObjectKey() = default;
        JsonObjectKey(const char* str, std::size_t len) : identifier(str, len) {}
    };

    struct JsonObjectEnd
    {
        static constexpr const char* name = "'}'";
    };

    struct JsonArrayStart
    {
        static constexpr const char* name = "'['";
    };

    struct JsonArrayEnd
    {
        static constexpr const char* name = "']'";
    };

    struct EventHandler
    {
        virtual ~EventHandler() {}
        virtual bool parse(const JsonValueNull&) = 0;
        virtual bool parse(const JsonValueBoolean&) = 0;
        virtual bool parse(const JsonValueNumber&) = 0;
        virtual bool parse(const JsonValueString&) = 0;
        virtual bool parse(const JsonObjectStart&) = 0;
        virtual bool parse(const JsonObjectKey&) = 0;
        virtual bool parse(const JsonObjectEnd&) = 0;
        virtual bool parse(const JsonArrayStart&) = 0;
        virtual bool parse(const JsonArrayEnd&) = 0;
    };
}
