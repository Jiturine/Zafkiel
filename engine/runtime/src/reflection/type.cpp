#include "type.h"

namespace Zafkiel::Reflection
{
std::string Numeric::GetNameOfKind(Kind kind)
{
    switch (kind)
    {
        using enum Kind;
    case Int8: return "Int8";
    case Int16: return "Int16";
    case Int32: return "Int32";
    case Int64: return "Int64";
    case Float: return "Float";
    case Double: return "Double";
    default: return "Unknown";
    }
}

Class &Class::AddProperty(const std::shared_ptr<Property> &prop)
{
    properties.emplace_back(prop);
    return *this;
}

}