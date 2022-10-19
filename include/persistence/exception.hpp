#pragma once
#include <stdexcept>
#include <string>

namespace persistence
{
    struct JsonSerializationError : std::runtime_error
    {
        JsonSerializationError() : std::runtime_error("serialization failed") {}
    };

    struct JsonDeserializationError : std::runtime_error
    {
        JsonDeserializationError()
            : std::runtime_error("deserialization failed")
        {}

        JsonDeserializationError(std::size_t offset)
            : std::runtime_error("deserialization failed at offset " + std::to_string(offset))
        {}
    };
}
