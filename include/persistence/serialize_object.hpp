#pragma once
#include "serialize_base.hpp"
#include "detail/serialize_aware.hpp"
#include "object.hpp"
#include <optional>

namespace persistence
{
    struct JsonObjectSerializer : JsonContextAwareSerializer
    {
        JsonObjectSerializer(SerializerContext& context, rapidjson::Value& object)
            : JsonContextAwareSerializer(context)
            , object(object)
        {}

        template<typename T>
        JsonObjectSerializer& operator&(const member_variable<std::optional<T>>& member)
        {
            if (member.ref.has_value()) {
                return write(member.name, member.ref.value());
            } else {
                return *this;
            }
        }

        template<typename T>
        JsonObjectSerializer& operator&(const member_variable<T>& member)
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
        JsonObjectSerializer& write(const std::string_view& name, const T& ref)
        {
            rapidjson::Value member_json;
            SerializerContext member_context(context, Segment(name));
            result = result && serialize(ref, member_json, member_context);
            if (result) {
                rapidjson::Value::StringRefType str(name.data(), name.size());
                object.AddMember(str, member_json, context.allocator());
            }
            return *this;
        }

        /** The JSON object into which data is written. */
        rapidjson::Value& object;

        /** The result of serializing the entire object. */
        bool result = true;
    };

    template<typename, typename = void>
    struct has_custom_serializer : std::false_type {};

    template<typename T>
    struct has_custom_serializer<T, std::void_t<
        decltype(
            std::declval<T&>().persist(std::declval<JsonObjectSerializer&>())
        )
    >> : std::true_type {};

    /**
    * Writes a value with a specific type to JSON.
    */
    template<typename T, typename Enable>
    struct JsonSerializer : JsonContextAwareSerializer {
        using JsonContextAwareSerializer::JsonContextAwareSerializer;

        bool operator()(const T& value, rapidjson::Value& json) const
        {
            static_assert(has_custom_serializer<T>::value, "expected a type that can be serialized to JSON");

            json.SetObject();

            JsonObjectSerializer serializer(context, json);
            const_cast<T&>(value).persist(serializer);
            return static_cast<bool>(serializer);
        }
    };
}
