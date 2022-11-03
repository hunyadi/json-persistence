#pragma once
#include "object.hpp"
#include "deserialize_base.hpp"
#include "deserialize_check.hpp"
#include "detail/deserialize_aware.hpp"
#include <optional>

namespace persistence
{
    template<bool Exception, typename C>
    struct JsonObjectDeserializer : JsonContextAwareDeserializer
    {
        JsonObjectDeserializer(DeserializerContext& context, const rapidjson::Value& json_object, C& object)
            : JsonContextAwareDeserializer(context)
            , json_object(json_object)
            , object(object)
        {}

        template<typename T>
        JsonObjectDeserializer& operator&(const member_variable<std::optional<T>, C>& member)
        {
            PERSISTENCE_IF_UNLIKELY(!result) {
                return *this;
            }

            auto it = json_object.FindMember(member.name.data());
            if (it != json_object.MemberEnd()) {
                T value;
                DeserializerContext value_context(context, Segment(it->name.GetString()));
                result = deserialize<Exception>(it->value, value, value_context);
                member.ref(object) = std::move(value);
            } else {
                member.ref(object) = std::nullopt;
            }
            return *this;
        }

        template<typename T>
        JsonObjectDeserializer& operator&(const member_variable<T, C>& member)
        {
            PERSISTENCE_IF_UNLIKELY(!result) {
                return *this;
            }

            auto it = json_object.FindMember(member.name.data());
            PERSISTENCE_IF_UNLIKELY(it == json_object.MemberEnd()) {
                if constexpr (Exception) {
                    throw JsonDeserializationError(
                        "missing required property: " + std::string(member.name),
                        Path(context.segments()).str()
                    );
                } else {
                    result = false;
                }
            }

            DeserializerContext value_context(context, Segment(it->name.GetString()));
            result = deserialize<Exception>(it->value, member.ref(object), value_context);
            return *this;
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
        /** The JSON object from which data is read. */
        const rapidjson::Value& json_object;

        /** The C++ object into which data is written. */
        C& object;

        /** The result of deserializing the entire object. */
        bool result = true;
    };

    template<typename T>
    using deserializer_function = decltype(std::declval<T&>().persist(std::declval<JsonObjectDeserializer<false, T>&>()));

    template<typename T, typename Enable = void>
    struct has_custom_deserializer : std::false_type
    {};

    template<typename T>
    struct has_custom_deserializer<T, typename std::enable_if<std::is_class<T>::value>::type>
    {
        constexpr static bool value = detect<T, deserializer_function>::value;
    };

    /**
    * Writes a value with a specific type to JSON.
    */
    template<bool Exception, typename T>
    struct JsonDeserializer<Exception, T, typename std::enable_if<has_custom_deserializer<T>::value>::type> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, T& value) const
        {
            if (!detail::check_object<Exception>(json, context)) {
                return false;
            }

            JsonObjectDeserializer<Exception, T> deserializer(context, json, value);
            value.persist(deserializer);
            return static_cast<bool>(deserializer);
        }
    };
}
