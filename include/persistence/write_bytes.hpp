#pragma once
#include "base64.hpp"
#include "write_base.hpp"
#include <string>

namespace persistence
{
    template<>
    struct JsonWriter<byte_vector>
    {
        bool operator()(const byte_vector& container, StringWriter& writer) const
        {
            std::string str;
            if (!base64_encode(container, str)) {
                return false;
            }
            writer.String(str.data(), static_cast<rapidjson::SizeType>(str.size()), true);
            return true;
        }
    };
}
