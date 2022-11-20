#pragma once
#include "parse_base.hpp"

namespace persistence
{
    /** Attempts to parse the JSON type associated with a given C++ type, or the end of array token. */
    template<typename T>
    struct JsonArrayItemParseHandler : JsonParseHandler<typename JsonParser<T>::json_type, JsonArrayEnd>
    {
        using JsonParseHandler<typename JsonParser<T>::json_type, JsonArrayEnd>::JsonParseHandler;
    };
}
