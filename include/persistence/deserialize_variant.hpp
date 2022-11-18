#pragma once
#include "deserialize_base.hpp"
#include "exception.hpp"
#include "detail/deserialize_aware.hpp"
#include "detail/deserialize_context.hpp"
#include "detail/path.hpp"
#include <variant>

namespace persistence
{
    template<bool Exception, typename... Ts>
    struct JsonDeserializer<Exception, std::variant<Ts...>> : JsonContextAwareDeserializer
    {
        using JsonContextAwareDeserializer::JsonContextAwareDeserializer;

        bool operator()(const rapidjson::Value& json, std::variant<Ts...>& value) const
        {
            if (!(deserialize_variant<Ts>(json, value) || ...)) {
                if constexpr (Exception) {
                    throw JsonDeserializationError(
                        "unable to match either of the variant types",
                        Path(context.segments()).str()
                    );
                } else {
                    return false;
                }
            }
            return true;
        }

    private:
        template<typename T>
        bool deserialize_variant(const rapidjson::Value& json, std::variant<Ts...>& value) const
        {
            T var;
            if (!deserialize<false>(json, var, context)) {
                return false;
            }

            value = std::move(var);
            return true;
        }
    };
}
