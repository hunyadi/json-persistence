#pragma once
#include "dictionary.hpp"
#include "write_base.hpp"
#include "detail/write_aware.hpp"
#include "detail/unlikely.hpp"
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
                writer.Key(key.data(), static_cast<rapidjson::SizeType>(key.size()), true);

                WriterContext value_context(context, Segment(key));
                PERSISTENCE_IF_UNLIKELY(!serialize<typename C::value_type::second_type>(value, writer, value_context)) {
                    return false;
                }
            }
            writer.EndObject();
            return true;
        }
    };

    template<typename T>
    struct JsonWriter<literal_dict<T>> : JsonDictionaryWriter<literal_dict<T>>
    {
        using JsonDictionaryWriter<literal_dict<T>>::JsonDictionaryWriter;
    };

    template<typename T>
    struct JsonWriter<literal_map<T>> : JsonDictionaryWriter<literal_map<T>>
    {
        using JsonDictionaryWriter<literal_map<T>>::JsonDictionaryWriter;
    };

    template<typename T>
    struct JsonWriter<literal_unordered_map<T>> : JsonDictionaryWriter<literal_unordered_map<T>>
    {
        using JsonDictionaryWriter<literal_unordered_map<T>>::JsonDictionaryWriter;
    };

    template<typename T>
    struct JsonWriter<string_dict<T>> : JsonDictionaryWriter<string_dict<T>>
    {
        using JsonDictionaryWriter<string_dict<T>>::JsonDictionaryWriter;
    };

    template<typename T>
    struct JsonWriter<string_map<T>> : JsonDictionaryWriter<string_map<T>>
    {
        using JsonDictionaryWriter<string_map<T>>::JsonDictionaryWriter;
    };

    template<typename T>
    struct JsonWriter<string_unordered_map<T>> : JsonDictionaryWriter<string_unordered_map<T>>
    {
        using JsonDictionaryWriter<string_unordered_map<T>>::JsonDictionaryWriter;
    };
}
