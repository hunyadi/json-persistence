#pragma once
#include <map>
#include <unordered_map>

namespace persistence
{
    template<typename C>
    struct JsonMapParser : EventHandler
    {
        JsonMapParser(ReaderContext& context, C& container)
            : context(context)
            , container(container)
        {}

        bool parse(JsonObjectEnd) override
        {
            context.pop();
            return true;
        }

        bool parse(JsonObjectKey json_key) override
        {
            using value_type = typename C::mapped_type;
            std::string_view key = json_key.identifier;
            auto&& [iter, ins] = container.try_emplace(std::string(key));
            value_type& item = iter->second;
            context.push(std::make_unique<JsonParser<value_type>>(context, item));
            return true;
        }

    private:
        ReaderContext& context;
        C& container;
    };

    template<typename C>
    struct JsonMappedTypeParser : EventHandler
    {
        using json_type = JsonObjectStart;

        JsonMappedTypeParser(ReaderContext& context, C& container)
            : context(context)
            , container(container)
        {}

        bool parse(JsonObjectStart) override
        {
            context.replace(std::make_unique<JsonMapParser<C>>(context, container));
            return true;
        }

    private:
        ReaderContext& context;
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
