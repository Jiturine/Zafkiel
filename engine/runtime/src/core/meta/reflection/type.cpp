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

std::any Array::GetElem(size_t index, const std::any &array) const
{
    return getElemFunc(index, array);
}
std::any Array::GetElemConst(size_t index, const std::any &array) const
{
    return getElemConstFunc(index, array);
}
std::any Array::GetBack(const std::any &array) const
{
    return getBackFunc(array);
}
std::any Array::GetBackConst(const std::any &array) const
{
    return getBackConstFunc(array);
}
size_t Array::GetSize(const std::any &array) const
{
    return getSizeFunc(array);
}
size_t Array::GetSizeConst(const std::any &array) const
{
    return getSizeConstFunc(array);
}

Class &Class::AddProperty(const std::shared_ptr<Property> &prop)
{
    properties.emplace_back(prop);
    return *this;
}

}