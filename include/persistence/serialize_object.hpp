#pragma once
#include "object.hpp"
#include "serialize_base.hpp"
#include "detail/serialize_aware.hpp"
#include <optional>

namespace persistence
{
    template<typename C>
    struct JsonObjectSerializer : JsonContextAwareSerializer
    {
        JsonObjectSerializer(SerializerContext& context, const C& object, rapidjson::Value& json_object)
            : JsonContextAwareSerializer(context)
            , object(object)
            , json_object(json_object)
        {}

        template<typename T>
        JsonObjectSerializer& operator&(const member_variable<std::optional<T>, C>& member)
        {
            if (member.ref(object).has_value()) {
                return write(member.name, member.ref(object).value());
            } else {
                return *this;
            }
        }

        template<typename T>
        JsonObjectSerializer& operator&(const member_variable<T, C>& member)
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
        JsonObjectSerializer& write(const std::string_view& name, const T& ref)
        {
            if (!result) {
                return *this;
            }

            rapidjson::Value member_json;
            SerializerContext member_context(context, Segment(name));
            result = result && serialize(ref, member_json, member_context);
            if (result) {
                rapidjson::Value::StringRefType str(name.data(), name.size());
                json_object.AddMember(str, member_json, context.global().allocator());
            }
            return *this;
        }

    private:
        /** The C++ object from which data is read. */
        const C& object;

        /** The JSON object into which data is written. */
        rapidjson::Value& json_object;

        /** The result of serializing the entire object. */
        bool result = true;
    };

    template<typename, typename = void>
    struct has_custom_serializer : std::false_type {};

    template<typename T>
    struct has_custom_serializer<T, std::void_t<
        decltype(
            std::declval<T&>().persist(std::declval<JsonObjectSerializer<T>&>())
        )
    >> : std::true_type {};

    /**
    * Writes a value with a specific type to JSON.
    */
    template<typename T, typename Enable>
    struct JsonSerializer : JsonContextAwareSerializer
    {
        using JsonContextAwareSerializer::JsonContextAwareSerializer;

        bool operator()(const T& value, rapidjson::Value& json) const
        {
            static_assert(has_custom_serializer<T>::value, "expected a type that can be serialized to JSON");

            JsonObjectSerializer<T> serializer(context, value, json);

            json.SetObject();
            const_cast<T&>(value).persist(serializer);
            return static_cast<bool>(serializer);
        }
    };
}
