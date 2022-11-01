#pragma once
#include "exception.hpp"
#include "detail/config.hpp"
#include "detail/deserialize_context.hpp"
#include <rapidjson/document.h>

namespace persistence
{
    namespace detail
    {
        template<bool Exception>
        bool check_string(const rapidjson::Value& json, DeserializerContext& context)
        {
            if (!json.IsString()) {
                if constexpr (Exception) {
                    throw JsonDeserializationError(
                        "wrong JSON data type; expected: string",
                        Path(context.segments()).str()
                    );
                } else {
                    return false;
                }
            }
            return true;
        }

        template<bool Exception>
        bool check_object(const rapidjson::Value& json, DeserializerContext& context)
        {
            if (!json.IsObject()) {
                if constexpr (Exception) {
                    throw JsonDeserializationError(
                        "wrong JSON data type; expected: object",
                        Path(context.segments()).str()
                    );
                } else {
                    return false;
                }
            }
            return true;
        }

        template<bool Exception>
        bool check_array(const rapidjson::Value& json, DeserializerContext& context)
        {
            if (!json.IsArray()) {
                if constexpr (Exception) {
                    throw JsonDeserializationError(
                        "wrong JSON data type; expected: array",
                        Path(context.segments()).str()
                    );
                } else {
                    return false;
                }
            }
            return true;
        }

        template<bool Exception, std::size_t N>
        bool check_size(const rapidjson::Value& json, DeserializerContext& context)
        {
            if (json.Size() != N) {
                if constexpr (Exception) {
                    throw JsonDeserializationError(
                        "mismatch in number of array elements; expected: " + std::to_string(N) + ", got: " + std::to_string(json.Size()),
                        Path(context.segments()).str()
                    );
                } else {
                    return false;
                }
            }
            return true;
        }
    }
}
