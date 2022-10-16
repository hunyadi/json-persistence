#pragma once
#include "object.hpp"
#include "write_base.hpp"
#include "detail/write_aware.hpp"
#include <optional>

namespace persistence
{
    struct JsonObjectWriter : JsonContextAwareWriter
    {
        JsonObjectWriter(WriterContext& context, StringWriter& writer)
            : JsonContextAwareWriter(context)
            , writer(writer)
        {}

        template<typename T>
        JsonObjectWriter& operator&(const member_variable<std::optional<T>>& member)
        {
            if (member.ref.has_value()) {
                return write(member.name, member.ref.value());
            } else {
                return *this;
            }
        }

        template<typename T>
        JsonObjectWriter& operator&(const member_variable<T>& member)
        {
            return write(member.name, member.ref);
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

        /** The JSON string buffer into which data is written. */
        StringWriter& writer;

        /** The result of serializing the entire object. */
        bool result = true;
    };

    template<typename, typename = void>
    struct has_custom_writer : std::false_type {};

    template<typename T>
    struct has_custom_writer<T, std::void_t<
        decltype(
            std::declval<T&>().persist(std::declval<JsonObjectWriter&>())
        )
    >> : std::true_type {};

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

            JsonObjectWriter serializer(context, writer);

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
