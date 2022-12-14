#pragma once
#include "serialize_base.hpp"
#include "detail/unlikely.hpp"
#include <cmath>

namespace persistence
{
    template<>
    struct JsonSerializer<std::nullptr_t>
    {
        bool operator()(std::nullptr_t, rapidjson::Value& json) const
        {
            json.SetNull();
            return true;
        }
    };

    template<>
    struct JsonSerializer<bool>
    {
        bool operator()(bool value, rapidjson::Value& json) const
        {
            json.SetBool(value);
            return true;
        }
    };

    template<typename T>
    struct JsonSignedIntegerSerializer
    {
        static_assert(std::is_integral_v<T> && std::is_signed_v<T>, "T must be a signed integer");

        bool operator()(T value, rapidjson::Value& json) const
        {
            json.SetInt(value);
            return true;
        }
    };

    template<typename T>
    struct JsonUnsignedIntegerSerializer
    {
        static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>, "T must be an unsigned integer");

        bool operator()(T value, rapidjson::Value& json) const
        {
            json.SetUint(value);
            return true;
        }
    };

    template<typename T>
    struct JsonSignedLongIntegerSerializer
    {
        static_assert(std::is_integral_v<T> && std::is_signed_v<T>, "T must be a signed integer");

        bool operator()(T value, rapidjson::Value& json) const
        {
            json.SetInt64(value);
            return true;
        }
    };

    template<typename T>
    struct JsonUnsignedLongIntegerSerializer
    {
        static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>, "T must be an unsigned integer");

        bool operator()(T value, rapidjson::Value& json) const
        {
            json.SetUint64(value);
            return true;
        }
    };

    template<>
    struct JsonSerializer<char> : std::conditional_t< std::is_signed_v<char>, JsonSignedIntegerSerializer<char>, JsonUnsignedIntegerSerializer<char> >
    {};

    template<>
    struct JsonSerializer<signed char> : JsonSignedIntegerSerializer<signed char>
    {};

    template<>
    struct JsonSerializer<unsigned char> : JsonUnsignedIntegerSerializer<unsigned char>
    {};

    template<>
    struct JsonSerializer<short> : JsonSignedIntegerSerializer<short>
    {};

    template<>
    struct JsonSerializer<unsigned short> : JsonUnsignedIntegerSerializer<unsigned short>
    {};

    template<>
    struct JsonSerializer<int> : JsonSignedIntegerSerializer<int>
    {};

    template<>
    struct JsonSerializer<unsigned int> : JsonUnsignedIntegerSerializer<unsigned int>
    {};

    template<>
    struct JsonSerializer<long> : JsonSignedLongIntegerSerializer<long>
    {};

    template<>
    struct JsonSerializer<unsigned long> : JsonUnsignedLongIntegerSerializer<unsigned long>
    {};

    template<>
    struct JsonSerializer<long long> : JsonSignedLongIntegerSerializer<long long>
    {};

    template<>
    struct JsonSerializer<unsigned long long> : JsonUnsignedLongIntegerSerializer<unsigned long long>
    {};

    template<typename T>
    struct JsonFloatSerializer
    {
        static_assert(std::is_floating_point_v<T>, "T must be a floating point type");

        bool operator()(T value, rapidjson::Value& json) const
        {
            PERSISTENCE_IF_UNLIKELY(!std::isfinite(value)) {
                // JSON specification does not allow for NaN, Inf, -Inf and subnormal values
                return false;
            }

            json.SetDouble(value);
            return true;
        }
    };

    template<>
    struct JsonSerializer<float> : JsonFloatSerializer<float>
    {};

    template<>
    struct JsonSerializer<double> : JsonFloatSerializer<double>
    {};
}
