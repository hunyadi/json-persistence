#pragma once
#include "write_base.hpp"
#include "detail/write_aware.hpp"
#include <variant>

namespace persistence
{
    template<typename... Ts>
    struct JsonWriter<std::variant<Ts...>> : JsonContextAwareWriter
    {
        using JsonContextAwareWriter::JsonContextAwareWriter;

        bool operator()(const std::variant<Ts...>& value, StringWriter& writer) const
        {
            return std::visit(
                [&](auto&& arg) -> bool {
                    return serialize(arg, writer, context);
                },
                value
            );
        }
    };
}
