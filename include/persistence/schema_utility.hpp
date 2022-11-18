#pragma once
#include "schema.hpp"
#include "write_fundamental.hpp"
#include "write_object.hpp"
#include "write_pointer.hpp"
#include "write_map.hpp"
#include "write_string.hpp"
#include "write_variant.hpp"
#include "write_vector.hpp"
#include "write.hpp"

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
