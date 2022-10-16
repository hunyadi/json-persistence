#pragma once
#include "datetime.hpp"
#include "write_base.hpp"

namespace persistence
{
    template<>
    struct JsonWriter<timestamp>
    {
        bool operator()(timestamp value, StringWriter& writer) const
        {
            auto dt = to_iso_datetime(value);
            writer.String(dt.text, iso_datetime::length(), true);
            return true;
        }
    };
}
