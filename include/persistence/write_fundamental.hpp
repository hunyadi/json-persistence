#pragma once
#include "write_base.hpp"
#include "detail/unlikely.hpp"
#include <cmath>

namespace persistence
{
    template<>
    struct JsonWriter<std::nullptr_t>
    {
        bool operator()(std::nullptr_t, StringWriter& writer) const
        {
            writer.Null();
            return true;
        }
    };

    template<>
    struct JsonWriter<bool>
    {
        bool operator()(bool value, StringWriter& writer) const
        {
            writer.Bool(value);
            return true;
        }
    };

    template<typename T>
    struct JsonSignedIntegerWriter
    {
        static_assert(std::is_integral_v<T> && std::is_signed_v<T>, "T must be a signed integer");

        bool operator()(T value, StringWriter& writer) const
        {
            writer.Int(value);
            return true;
        }
    };

    template<typename T>
    struct JsonUnsignedIntegerWriter
    {
        static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>, "T must be an unsigned integer");

        bool operator()(T value, StringWriter& writer) const
        {
            writer.Uint(value);
            return true;
        }
    };

    template<typename T>
    struct JsonSignedLongIntegerWriter
    {
        static_assert(std::is_integral_v<T> && std::is_signed_v<T>, "T must be a signed integer");

        bool operator()(T value, StringWriter& writer) const
        {
            writer.Int64(value);
            return true;
        }
    };

    template<typename T>
    struct JsonUnsignedLongIntegerWriter
    {
        static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>, "T must be an unsigned integer");

        bool operator()(T value, StringWriter& writer) const
        {
            writer.Uint64(value);
            return true;
        }
    };

    template<>
    struct JsonWriter<char> : std::conditional_t< std::is_signed_v<char>, JsonSignedIntegerWriter<char>, JsonUnsignedIntegerWriter<char> >
    {};

    template<>
    struct JsonWriter<signed char> : JsonSignedIntegerWriter<signed char>
    {};

    template<>
    struct JsonWriter<unsigned char> : JsonUnsignedIntegerWriter<unsigned char>
    {};

    template<>
    struct JsonWriter<short> : JsonSignedIntegerWriter<short>
    {};

    template<>
    struct JsonWriter<unsigned short> : JsonUnsignedIntegerWriter<unsigned short>
    {};

    template<>
    struct JsonWriter<int> : JsonSignedIntegerWriter<int>
    {};

    template<>
    struct JsonWriter<unsigned int> : JsonUnsignedIntegerWriter<unsigned int>
    {};

    template<>
    struct JsonWriter<long> : JsonSignedLongIntegerWriter<long>
    {};

    template<>
    struct JsonWriter<unsigned long> : JsonUnsignedLongIntegerWriter<unsigned long>
    {};

    template<>
    struct JsonWriter<long long> : JsonSignedLongIntegerWriter<long long>
    {};

    template<>
    struct JsonWriter<unsigned long long> : JsonUnsignedLongIntegerWriter<unsigned long long>
    {};

    template<typename T>
    struct JsonFloatWriter
    {
        static_assert(std::is_floating_point_v<T>, "T must be a floating point type");

        bool operator()(T value, StringWriter& writer) const
        {
            PERSISTENCE_IF_UNLIKELY(!std::isfinite(value)) {
                // JSON specification does not allow for NaN, Inf, -Inf and subnormal values
                return false;
            }

            writer.Double(value);
            return true;
        }
    };

    template<>
    struct JsonWriter<float> : JsonFloatWriter<float>
    {};

    template<>
    struct JsonWriter<double> : JsonFloatWriter<double>
    {};
}
