#pragma once
#include "detail/engine.hpp"
#include "deserialize_base.hpp"
#include "deserialize.hpp"
#include "object.hpp"
#include <optional>

namespace persistence
{
    struct JsonObjectDeserializer
    {
        JsonObjectDeserializer(const rapidjson::Value& object) : object(object)
        {}

        template<typename T>
        JsonObjectDeserializer& operator&(const member_variable<std::optional<T>>& member)
        {
            if (!result) {
                return *this;
            }

            auto it = object.FindMember(member.name.data());
            if (it != object.MemberEnd()) {
                T value;
                result = deserialize<T>(it->value, value);
                member.ref = std::move(value);
            }
            return *this;
        }

        template<typename T>
        JsonObjectDeserializer& operator&(const member_variable<T>& member)
        {
            if (!result) {
                return *this;
            }

            auto it = object.FindMember(member.name.data());
            if (it != object.MemberEnd()) {
                result = deserialize<T>(it->value, member.ref);
            } else {
                result = false;
            }
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
        const rapidjson::Value& object;

        /** The result of deserializing the entire object. */
        bool result = true;
    };

    template<typename, typename = void>
    struct has_custom_deserializer : std::false_type {};

    template<typename T>
    struct has_custom_deserializer<T, std::void_t<
        decltype(
            std::declval<T&>().persist(std::declval<JsonObjectDeserializer&>())
        )
        >> : std::true_type {};

    /**
    * Writes a value with a specific type to JSON.
    */
    template<typename T, typename Enable>
    struct JsonDeserializer {
        bool operator()(const rapidjson::Value& json, T& value) const
        {
            static_assert(has_custom_deserializer<T>::value, "expected a type that can be deserialized from JSON");

            if (!json.IsObject()) {
                return false;
            }

            JsonObjectDeserializer deserializer(json);
            value.persist(deserializer);
            return static_cast<bool>(deserializer);
        }
    };
}
