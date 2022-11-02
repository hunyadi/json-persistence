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
        rapidjson::ParseResult parse(const std::string& str, T& value)
        {
            EventDispatcher dispatcher;
            ReaderContext context(dispatcher);
            context.emplace<JsonParser<T>>(context, value);

            rapidjson::Reader reader;
            rapidjson::StringStream stream(str.data());
            return reader.Parse<rapidjson::kParseNumbersAsStringsFlag>(stream, dispatcher);
        }
    }

    template<typename T>
    bool parse(const std::string& str, T& value)
    {
        auto result = detail::parse(str, value);
        return !result.IsError();
    }

    template<typename T>
    T parse(const std::string& str)
    {
        T obj;
        auto result = detail::parse(str, obj);
        if (result.IsError()) {
            throw JsonParseError(rapidjson::GetParseError_En(result.Code()), result.Offset());
        }
        return obj;
    }
}
