#pragma once
#include <stdexcept>
#include <string>

namespace persistence
{
    /** Raised when a C++ object cannot be serialized to JSON DOM or JSON string. */
    struct JsonSerializationError : std::runtime_error
    {
        JsonSerializationError()
            : std::runtime_error("serialization failed")
        {}
    };

    /** Raised when invalid syntax is encountered in the JSON string. */
    struct JsonParseError : std::runtime_error
    {
        JsonParseError()
            : std::runtime_error("parse error")
        {}

        JsonParseError(const std::string& reason, std::size_t offset)
            : std::runtime_error("parse error at offset " + std::to_string(offset) + ": " + reason)
        {}
    };

    /** Raised when a C++ object cannot be reconstructed from JSON DOM or JSON string. */
    struct JsonDeserializationError : std::runtime_error
    {
        JsonDeserializationError(const std::string& reason, const std::string& path)
            : std::runtime_error(reason + " at " + path)
        {}

        JsonDeserializationError(const std::string& reason, std::size_t offset)
            : std::runtime_error(reason + " at offset " + std::to_string(offset))
        {}
    };
}
