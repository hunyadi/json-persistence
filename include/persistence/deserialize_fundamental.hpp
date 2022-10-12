#pragma once
#include "deserialize_base.hpp"
#include "detail/engine.hpp"

namespace persistence
{
    template<typename T>
    struct JsonSignedIntegerDeserializer
    {
        static_assert(std::is_integral<T>::value&& std::is_signed<T>::value, "T must be a signed integer");

        bool operator()(const rapidjson::Value& json, T& value) const
        {
            if (json.IsInt()) {
                auto integer_value = json.GetInt();
                if (integer_value >= std::numeric_limits<T>::min() && integer_value <= std::numeric_limits<T>::max()) {
                    value = static_cast<T>(integer_value);
                    return true;
                }
            }
            return false;
        }
    };

    template<typename T>
    struct JsonUnsignedIntegerDeserializer
    {
        static_assert(std::is_integral<T>::value&& std::is_unsigned<T>::value, "T must be an unsigned integer");

        bool operator()(const rapidjson::Value& json, T& value) const
        {
            if (json.IsUint()) {
                auto integer_value = json.GetUint();
                if (integer_value <= std::numeric_limits<T>::max()) {
                    value = static_cast<T>(integer_value);
                    return true;
                }
            }
            return false;
        }
    };

    template<typename T>
    struct JsonSignedLongIntegerDeserializer
    {
        static_assert(std::is_integral<T>::value&& std::is_signed<T>::value, "T must be a signed integer");

        bool operator()(const rapidjson::Value& json, T& value) const
        {
            if (json.IsInt64()) {
                auto integer_value = json.GetInt64();
                if (integer_value >= std::numeric_limits<T>::min() && integer_value <= std::numeric_limits<T>::max()) {
                    value = static_cast<T>(integer_value);
                    return true;
                }
            }
            return false;
        }
    };

    template<typename T>
    struct JsonUnsignedLongIntegerDeserializer
    {
        static_assert(std::is_integral<T>::value&& std::is_unsigned<T>::value, "T must be an unsigned integer");

        bool operator()(const rapidjson::Value& json, T& value) const
        {
            if (json.IsUint64()) {
                auto integer_value = json.GetUint64();
                if (integer_value <= std::numeric_limits<T>::max()) {
                    value = static_cast<T>(integer_value);
                    return true;
                }
            }
            return false;
        }
    };

    template<>
    struct JsonDeserializer<std::nullptr_t>
    {
        bool operator()(const rapidjson::Value& json, std::nullptr_t& value) const
        {
            if (json.IsNull()) {
                value = nullptr;
                return true;
            } else {
                return false;
            }
        }
    };

    template<>
    struct JsonDeserializer<bool>
    {
        bool operator()(const rapidjson::Value& json, bool& value) const
        {
            if (json.IsBool()) {
                value = json.GetBool();
                return true;
            } else {
                return false;
            }
        }
    };

    template<>
    struct JsonDeserializer<char> : std::conditional< std::is_signed<char>::value, JsonSignedIntegerDeserializer<char>, JsonUnsignedIntegerDeserializer<char> >::type
    {};

    template<>
    struct JsonDeserializer<signed char> : JsonSignedIntegerDeserializer<signed char>
    {};

    template<>
    struct JsonDeserializer<unsigned char> : JsonUnsignedIntegerDeserializer<unsigned char>
    {};

    template<>
    struct JsonDeserializer<short> : JsonSignedIntegerDeserializer<short>
    {};

    template<>
    struct JsonDeserializer<unsigned short> : JsonUnsignedIntegerDeserializer<unsigned short>
    {};

    template<>
    struct JsonDeserializer<int> : JsonSignedIntegerDeserializer<int>
    {};

    template<>
    struct JsonDeserializer<unsigned int> : JsonUnsignedIntegerDeserializer<unsigned int>
    {};

    template<>
    struct JsonDeserializer<long> : JsonSignedLongIntegerDeserializer<long>
    {};

    template<>
    struct JsonDeserializer<unsigned long> : JsonUnsignedLongIntegerDeserializer<unsigned long>
    {};

    template<>
    struct JsonDeserializer<long long> : JsonSignedLongIntegerDeserializer<long long>
    {};

    template<>
    struct JsonDeserializer<unsigned long long> : JsonUnsignedLongIntegerDeserializer<unsigned long long>
    {};

    template<typename T>
    struct JsonFloatDeserializer
    {
        bool operator()(const rapidjson::Value& json, T& value) const
        {
            if (json.IsNumber()) {
                value = static_cast<T>(json.GetDouble());
                return true;
            } else {
                return false;
            }
        }
    };

    template<>
    struct JsonDeserializer<float> : JsonFloatDeserializer<float> {};

    template<>
    struct JsonDeserializer<double> : JsonFloatDeserializer<double> {};
}
