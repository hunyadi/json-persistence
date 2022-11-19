#pragma once
#include "parse_base.hpp"
#include <map>
#include <unordered_map>

namespace persistence
{
    template<typename C>
    struct JsonMapParser : JsonParseHandler
    {
        JsonMapParser(ReaderContext& context, C& container)
            : JsonParseHandler(context)
            , container(container)
        {}

        bool parse(const JsonObjectEnd&) override
        {
            context.pop();
            return true;
        }

        bool parse(const JsonObjectKey& json_key) override
        {
            using value_type = typename C::mapped_type;
            std::string_view key = json_key.identifier;
            auto&& [iter, ins] = container.try_emplace(std::string(key));
            value_type& item = iter->second;
            context.emplace<JsonParser<value_type>>(context, item);
            return true;
        }

    private:
        C& container;
    };

    template<typename C>
    struct JsonMappedTypeParser : JsonParseSingleHandler<JsonObjectStart>
    {
        using json_type = JsonObjectStart;

        JsonMappedTypeParser(ReaderContext& context, C& container)
            : JsonParseSingleHandler<JsonObjectStart>(context)
            , container(container)
        {}

        bool parse(const JsonObjectStart&) override
        {
            context.replace<JsonMapParser<C>>(context, container);
            return true;
        }

    private:
        C& container;
    };

    template<typename T>
    struct JsonParser<std::map<std::string, T>> : JsonMappedTypeParser<std::map<std::string, T>>
    {
        using JsonMappedTypeParser<std::map<std::string, T>>::JsonMappedTypeParser;
    };

    template<typename T>
    struct JsonParser<std::unordered_map<std::string, T>> : JsonMappedTypeParser<std::unordered_map<std::string, T>>
    {
        using JsonMappedTypeParser<std::unordered_map<std::string, T>>::JsonMappedTypeParser;
    };
}
