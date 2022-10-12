#pragma once

namespace persistence
{
    /**
    * Writes a value with a specific type to JSON.
    */
    template<typename T, typename Enable = void>
    struct JsonSerializer;
}
