#pragma once

namespace persistence
{
    /**
    * Reads a value with a specific type from JSON.
    */
    template<typename T, typename Enable = void>
    struct JsonDeserializer;
}
