#pragma once
#include "object.hpp"
#include "serialize_base.hpp"
#include "detail/serialize_aware.hpp"
#include "detail/unlikely.hpp"
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

        template<typename T, typename B>
        JsonObjectSerializer& operator&(const member_variable<std::optional<T>, B>& member)
        {
            static_assert(std::is_base_of_v<B, C>, "expected a member variable part of the class inheritance chain");
            if (member.ref(object).has_value()) {
                return write(member.name, member.ref(object).value());
            } else {
                return *this;
            }
        }

        template<typename T, typename B>
        JsonObjectSerializer& operator&(const member_variable<T, B>& member)
        {
            static_assert(std::is_base_of_v<B, C>, "expected a member variable part of the class inheritance chain");
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
            PERSISTENCE_IF_UNLIKELY(!result) {
                return *this;
            }

            rapidjson::Value member_json;
            SerializerContext member_context(context, Segment(name));
            result = serialize(ref, member_json, member_context);
            if (result) {
                rapidjson::Value::StringRefType str(name.data(), static_cast<rapidjson::SizeType>(name.size()));
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

    template<typename T>
    using serializer_function = decltype(std::declval<T&>().persist(std::declval<JsonObjectSerializer<T>&>()));

    template<typename T, typename Enable = void>
    struct has_custom_serializer : std::false_type
    {};

    template<typename T>
    struct has_custom_serializer<T, std::enable_if_t<std::is_class_v<T>>>
    {
        constexpr static bool value = detect<T, serializer_function>::value;
    };

    /**
    * Writes a value with a specific type to JSON.
    */
    template<typename T>
    struct JsonSerializer<T, std::enable_if_t<has_custom_serializer<T>::value>> : JsonContextAwareSerializer
    {
        using JsonContextAwareSerializer::JsonContextAwareSerializer;

        bool operator()(const T& value, rapidjson::Value& json) const
        {
            JsonObjectSerializer<T> serializer(context, value, json);

            json.SetObject();
            const_cast<T&>(value).persist(serializer);
            return static_cast<bool>(serializer);
        }
    };
}
