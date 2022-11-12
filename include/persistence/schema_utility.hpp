#pragma once
#include "schema.hpp"
#include "write_all.hpp"

namespace persistence
{
    template<typename T>
    std::string schema_to_string()
    {
        return write_to_string(schema<T>());
    }

    template<typename T>
    std::string schema_document_to_string()
    {
        return write_to_string(schema_document<T>());
    }
}
