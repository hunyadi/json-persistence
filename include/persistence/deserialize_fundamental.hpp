#pragma once
#include "deserialize_base.hpp"
#include "detail/deserialize_aware.hpp"
#include "detail/deserialize_context.hpp"
#include "detail/path.hpp"
#include "detail/unlikely.hpp"

namespace persistence
{
    namespace detail
    {
        template<bool Exception, typename NarrowType, typename WideType>
        bool check_range(WideType integer_value, DeserializerContext& context)
        {
            if constexpr (std::is_signed_v<NarrowType>) {
                PERSISTENCE_IF_UNLIKELY(integer_value < std::numeric_limits<NarrowType>::min() || integer_value > std::numeric_limits<NarrowType>::max()) {
                    if constexpr (Exception) {
                        throw JsonDeserializationError(
                            "signed integer out of range",
                            Path(context.segments()).str()
                        );
                    } else {
                        return false;
                    }
                }
            } else {
                PERSISTENCE_IF_UNLIKELY(integer_value > std::numeric_limits<NarrowType>::max()) {
                    if constexpr (Exception) {
                        throw JsonDeserializationError(
                            "unsigned integer out of range",
                            Path(context.segments()).str()
                        );
                    } else {
                        return false;
                    }
                }
            }
            return true;
        }
    }

    template<bool Exception, typename T>
    struct JsonSignedIntegerDeserializer : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        static_assert(std::is_integral_v<T> && std::is_signed_v<T>, "T must be a signed integer");

        bool operator()(const rapidjson::Value& json, T& value) const
        {
            PERSISTENCE_IF_UNLIKELY(!json.IsInt()) {
                if constexpr (Exception) {
                    throw JsonDeserializationError(
                        "wrong JSON data type; expected: integer",
                        Path(context.segments()).str()
                    );
                } else {
                    return false;
                }
            }

            auto integer_value = json.GetInt();
            if (!detail::check_range<Exception, T>(integer_value, context)) {
                return false;
            }

            value = static_cast<T>(integer_value);
            return true;
        }
    };

    template<bool Exception, typename T>
    struct JsonUnsignedIntegerDeserializer : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>, "T must be an unsigned integer");

        bool operator()(const rapidjson::Value& json, T& value) const
        {
            PERSISTENCE_IF_UNLIKELY(!json.IsUint()) {
                if constexpr (Exception) {
                    throw JsonDeserializationError(
                        "wrong JSON data type; expected: unsigned integer",
                        Path(context.segments()).str()
                    );
                } else {
                    return false;
                }
            }

            auto integer_value = json.GetUint();
            if (!detail::check_range<Exception, T>(integer_value, context)) {
                return false;
            }

            value = static_cast<T>(integer_value);
            return true;
        }
    };

    template<bool Exception, typename T>
    struct JsonSignedLongIntegerDeserializer : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        static_assert(std::is_integral_v<T> && std::is_signed_v<T>, "T must be a signed integer");

        bool operator()(const rapidjson::Value& json, T& value) const
        {
            PERSISTENCE_IF_UNLIKELY(!json.IsInt64()) {
                if constexpr (Exception) {
                    throw JsonDeserializationError(
                        "wrong JSON data type; expected: 64-bit integer",
                        Path(context.segments()).str()
                    );
                } else {
                    return false;
                }
            }

            auto integer_value = json.GetInt64();
            if (!detail::check_range<Exception, T>(integer_value, context)) {
                return false;
            }

            value = static_cast<T>(integer_value);
            return true;
        }
    };

    template<bool Exception, typename T>
    struct JsonUnsignedLongIntegerDeserializer : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>, "T must be an unsigned integer");

        bool operator()(const rapidjson::Value& json, T& value) const
        {
            PERSISTENCE_IF_UNLIKELY(!json.IsUint64()) {
                if constexpr (Exception) {
                    throw JsonDeserializationError(
                        "wrong JSON data type; expected: unsigned 64-bit integer",
                        Path(context.segments()).str()
                    );
                } else {
                    return false;
                }
            }

            auto integer_value = json.GetUint64();
            if (!detail::check_range<Exception, T>(integer_value, context)) {
                return false;
            }

            value = static_cast<T>(integer_value);
            return true;
        }
    };

    template<bool Exception>
    struct JsonDeserializer<Exception, std::nullptr_t> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, std::nullptr_t& value) const
        {
            PERSISTENCE_IF_UNLIKELY(!json.IsNull()) {
                if constexpr (Exception) {
                    throw JsonDeserializationError(
                        "wrong JSON data type; expected: null",
                        Path(context.segments()).str()
                    );
                } else {
                    return false;
                }
            }

            value = nullptr;
            return true;
        }
    };

    template<bool Exception>
    struct JsonDeserializer<Exception, bool> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, bool& value) const
        {
            PERSISTENCE_IF_UNLIKELY(!json.IsBool()) {
                if constexpr (Exception) {
                    throw JsonDeserializationError(
                        "wrong JSON data type; expected: boolean",
                        Path(context.segments()).str()
                    );
                } else {
                    return false;
                }
            }

            value = json.GetBool();
            return true;
        }
    };

    template<bool Exception>
    struct JsonDeserializer<Exception, signed char> : JsonSignedIntegerDeserializer<Exception, signed char>
    {
        using JsonSignedIntegerDeserializer<Exception, signed char>::JsonSignedIntegerDeserializer;
    };

    template<bool Exception>
    struct JsonDeserializer<Exception, unsigned char> : JsonUnsignedIntegerDeserializer<Exception, unsigned char>
    {
        using JsonUnsignedIntegerDeserializer<Exception, unsigned char>::JsonUnsignedIntegerDeserializer;
    };

    template<bool Exception>
    struct JsonDeserializer<Exception, char> : JsonSignedIntegerDeserializer<Exception, char>
    {
        using JsonSignedIntegerDeserializer<Exception, char>::JsonSignedIntegerDeserializer;
    };

    template<bool Exception>
    struct JsonDeserializer<Exception, short> : JsonSignedIntegerDeserializer<Exception, short>
    {
        using JsonSignedIntegerDeserializer<Exception, short>::JsonSignedIntegerDeserializer;
    };

    template<bool Exception>
    struct JsonDeserializer<Exception, unsigned short> : JsonUnsignedIntegerDeserializer<Exception, unsigned short>
    {
        using JsonUnsignedIntegerDeserializer<Exception, unsigned short>::JsonUnsignedIntegerDeserializer;
    };

    template<bool Exception>
    struct JsonDeserializer<Exception, int> : JsonSignedIntegerDeserializer<Exception, int>
    {
        using JsonSignedIntegerDeserializer<Exception, int>::JsonSignedIntegerDeserializer;
    };

    template<bool Exception>
    struct JsonDeserializer<Exception, unsigned int> : JsonUnsignedIntegerDeserializer<Exception, unsigned int>
    {
        using JsonUnsignedIntegerDeserializer<Exception, unsigned int>::JsonUnsignedIntegerDeserializer;
    };

    template<bool Exception>
    struct JsonDeserializer<Exception, long> : JsonSignedLongIntegerDeserializer<Exception, long>
    {
        using JsonSignedLongIntegerDeserializer<Exception, long>::JsonSignedLongIntegerDeserializer;
    };

    template<bool Exception>
    struct JsonDeserializer<Exception, unsigned long> : JsonUnsignedLongIntegerDeserializer<Exception, unsigned long>
    {
        using JsonUnsignedLongIntegerDeserializer<Exception, unsigned long>::JsonUnsignedLongIntegerDeserializer;
    };

    template<bool Exception>
    struct JsonDeserializer<Exception, long long> : JsonSignedLongIntegerDeserializer<Exception, long long>
    {
        using JsonSignedLongIntegerDeserializer<Exception, long long>::JsonSignedLongIntegerDeserializer;
    };

    template<bool Exception>
    struct JsonDeserializer<Exception, unsigned long long> : JsonUnsignedLongIntegerDeserializer<Exception, unsigned long long>
    {
        using JsonUnsignedLongIntegerDeserializer<Exception, unsigned long long>::JsonUnsignedLongIntegerDeserializer;
    };

    template<bool Exception, typename T>
    struct JsonFloatDeserializer : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        static_assert(std::is_floating_point_v<T>, "T must be a floating-point type");

        bool operator()(const rapidjson::Value& json, T& value) const
        {
            PERSISTENCE_IF_UNLIKELY(!json.IsNumber()) {
                if constexpr (Exception) {
                    throw JsonDeserializationError(
                        "wrong JSON data type; expected: number",
                        Path(context.segments()).str()
                    );
                } else {
                    return false;
                }
            }

            value = static_cast<T>(json.GetDouble());
            return true;
        }
    };

    template<bool Exception>
    struct JsonDeserializer<Exception, float> : JsonFloatDeserializer<Exception, float>
    {
        using JsonFloatDeserializer<Exception, float>::JsonFloatDeserializer;
    };

    template<bool Exception>
    struct JsonDeserializer<Exception, double> : JsonFloatDeserializer<Exception, double>
    {
        using JsonFloatDeserializer<Exception, double>::JsonFloatDeserializer;
    };
}
