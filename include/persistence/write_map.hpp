#pragma once
#include "write_base.hpp"
#include "detail/write_aware.hpp"
#include <map>
#include <unordered_map>

namespace persistence
{
    template<typename C>
    struct JsonDictionaryWriter : JsonContextAwareWriter
    {
        using JsonContextAwareWriter::JsonContextAwareWriter;

        bool operator()(const C& container, StringWriter& writer) const
        {
            writer.StartObject();
            for (auto&& [key, value] : container) {
                writer.Key(key.data(), key.size(), true);

                WriterContext value_context(context, Segment(key));
                if (!serialize<typename C::mapped_type>(value, writer, value_context)) {
                    return false;
                }
            }
            writer.EndObject();
            return true;
        }
    };

    template<typename T>
    struct JsonWriter<std::map<std::string, T>> : JsonDictionaryWriter<std::map<std::string, T>>
    {
        using JsonDictionaryWriter<std::map<std::string, T>>::JsonDictionaryWriter;
    };

    template<typename T>
    struct JsonWriter<std::unordered_map<std::string, T>> : JsonDictionaryWriter<std::unordered_map<std::string, T>>
    {
        using JsonDictionaryWriter<std::unordered_map<std::string, T>>::JsonDictionaryWriter;
    };
}
