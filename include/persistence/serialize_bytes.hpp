#pragma once
#include "serialize_base.hpp"
#include "detail/serialize_aware.hpp"
#include "detail/unlikely.hpp"
#include "base64.hpp"
#include <vector>

namespace persistence
{
    template<>
    struct JsonSerializer<byte_vector> : JsonContextAwareSerializer
    {
        using JsonContextAwareSerializer::JsonContextAwareSerializer;

        bool operator()(const byte_vector& container, rapidjson::Value& json) const
        {
            std::string str;
            PERSISTENCE_IF_UNLIKELY(!base64_encode(container, str)) {
                return false;
            }

            json.SetString(str.data(), static_cast<rapidjson::SizeType>(str.size()), context.global().allocator());
            return true;
        }
    };
}
