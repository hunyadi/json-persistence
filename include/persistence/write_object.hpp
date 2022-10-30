#pragma once
#include "object.hpp"
#include "write_base.hpp"
#include "detail/write_aware.hpp"
#include <optional>

namespace persistence
{
    template<typename C>
    struct JsonObjectWriter : JsonContextAwareWriter
    {
        JsonObjectWriter(WriterContext& context, const C& object, StringWriter& writer)
            : JsonContextAwareWriter(context)
            , object(object)
            , writer(writer)
        {}

        template<typename T>
        JsonObjectWriter& operator&(const member_variable<std::optional<T>, C>& member)
        {
            if (member.ref(object).has_value()) {
                return write(member.name, member.ref(object).value());
            } else {
                return *this;
            }
        }

        template<typename T>
        JsonObjectWriter& operator&(const member_variable<T, C>& member)
        {
            return write(member.name, member.ref(object));
        }

        operator bool() const
        {
            return result;
        }

        bool operator!() const
        {
            return !result;
        }

    private:
        template<typename T>
        JsonObjectWriter& write(const std::string_view& name, const T& ref)
        {
            if (!result) {
                return *this;
            }

            writer.Key(name.data(), name.size(), false);

            WriterContext member_context(context, Segment(name));
            result = result && serialize(ref, writer, member_context);
            return *this;
        }

    private:
        /** The C++ object from which data is read. */
        const C& object;

        /** The JSON string buffer into which data is written. */
        StringWriter& writer;

        /** The result of serializing the entire object. */
        bool result = true;
    };

    template<typename T>
    using writer_function = decltype(std::declval<T&>().persist(std::declval<JsonObjectWriter<T>&>()));

    template<typename T, typename Enable = void>
    struct has_custom_writer : std::false_type
    {};

    template<typename T>
    struct has_custom_writer<T, typename std::enable_if<std::is_class<T>::value>::type>
    {
        constexpr static bool value = detect<T, writer_function>::value;
    };

    /**
    * Writes a value with a specific type to JSON.
    */
    template<typename T, typename Enable>
    struct JsonWriter : JsonContextAwareWriter
    {
        using JsonContextAwareWriter::JsonContextAwareWriter;

        bool operator()(const T& value, StringWriter& writer) const
        {
            static_assert(has_custom_writer<T>::value, "expected a type that can be serialized to JSON");

            JsonObjectWriter serializer(context, value, writer);

            writer.StartObject();
            const_cast<T&>(value).persist(serializer);
            if (!serializer) {
                return false;
            }
            writer.EndObject();
            return true;
        }
    };
}
