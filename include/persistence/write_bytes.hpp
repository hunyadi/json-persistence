#pragma once
#include "write_base.hpp"
#include "base64.hpp"

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
            writer.String(str.data(), str.size(), true);
            return true;
        }
    };
}
