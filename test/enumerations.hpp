#include "persistence/enum.hpp"
#include "persistence/enum_utility.hpp"

enum class Esper { Unu, Du, Tri, Kvar, Kvin, Ses, Sep, Ok, Naux, Dek };

namespace persistence
{
    template<>
    struct enum_traits<Esper>
    {
        inline static constexpr auto converter = make_enum_converter({
            make_enum(Esper::Unu, "Unu"),
            make_enum(Esper::Du, "Du"),
            make_enum(Esper::Tri, "Tri"),
            make_enum(Esper::Kvar, "Kvar"),
            make_enum(Esper::Kvin, "Kvin"),
            make_enum(Esper::Ses, "Ses"),
            make_enum(Esper::Sep, "Sep"),
            make_enum(Esper::Ok, "Ok"),
            make_enum(Esper::Naux, "Naux"),
            make_enum(Esper::Dek, "Dek")
        });

        static std::string_view to_string(Esper value)
        {
            return converter.to_string(value);
        }

        static bool from_string(const std::string_view& name, Esper& value)
        {
            return converter.from_string(name, value);
        }
    };
}
