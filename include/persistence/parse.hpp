#pragma once
#include "parse_base.hpp"
#include "exception.hpp"
#include <rapidjson/reader.h>
#include <rapidjson/error/en.h>

namespace persistence
{
    namespace detail
    {
        template<typename T>
        rapidjson::ParseResult parse(ReaderContext& context, const std::string& str, T& value)
        {
            context.emplace<JsonParser<T>>(context, value);
            rapidjson::Reader reader;
            rapidjson::StringStream stream(str.data());

            // pass flag `kParseNumbersAsStringsFlag` as non-type template argument to prevent
            // rapidjson from parsing numbers and emitting type-specific events
            return reader.Parse(stream, context.dispatcher);
        }
    }

    template<typename T>
    bool parse(const std::string& str, T& value)
    {
        JsonParseEventDispatcher dispatcher;
        ReaderContext context(dispatcher);
        auto result = detail::parse(context, str, value);
        return !result.IsError();
    }

    template<typename T>
    T parse(const std::string& str)
    {
        static_assert(!std::is_const_v<T> && !std::is_volatile_v<T> && !std::is_reference_v<T>, "expected a type without qualifiers");

        T obj;
        JsonParseEventDispatcher dispatcher;
        ReaderContext context(dispatcher);
        auto result = detail::parse(context, str, obj);
        if (result.IsError()) {
            if (context.has_error()) {
                throw JsonParseError(context.get_error(), result.Offset());
            } else {
                throw JsonParseError(rapidjson::GetParseError_En(result.Code()), result.Offset());
            }
        }
        return obj;
    }
}
