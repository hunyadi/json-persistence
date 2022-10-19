#pragma once
#include "read_base.hpp"
#include "exception.hpp"
#include <rapidjson/reader.h>

namespace persistence
{
    template<typename T>
    bool parse(const std::string& str, T& value)
    {
        EventDispatcher dispatcher;
        ReaderContext context(dispatcher);
        context.push(std::make_unique<JsonParser<T>>(context, value));

        rapidjson::Reader reader;
        rapidjson::StringStream stream(str.data());
        return reader.Parse<rapidjson::kParseNumbersAsStringsFlag>(stream, dispatcher);
    }

    template<typename T>
    T parse(const std::string& str)
    {
        T obj;
        if (!parse(str, obj)) {
            throw JsonDeserializationError();
        }
        return obj;
    }
}
