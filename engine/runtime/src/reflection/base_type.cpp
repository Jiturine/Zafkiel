#include "base_type.h"
#include <cstdint>
#include "any.h"
#include <cassert>
#include "make_any.h"

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

void Numeric::SetValue(int64_t value, const Any &elem) const
{
    if (elem.typeinfo->GetKind() == Type::Kind::Numeric)
    {
        auto type = elem.typeinfo->As<Numeric>();
        switch (type->GetKind())
        {
            using enum Kind;
        case Int8:
            if (type->IsSigned()) { *(int8_t *)elem.payload = value; }
            else { *(uint8_t *)elem.payload = value; }
            break;
        case Int16:
            if (type->IsSigned()) { *(int16_t *)elem.payload = value; }
            else { *(uint16_t *)elem.payload = value; }
            break;
        case Int32:
            if (type->IsSigned()) { *(int32_t *)elem.payload = value; }
            else { *(uint32_t *)elem.payload = value; }
            break;
        case Int64:
            if (type->IsSigned()) { *(int64_t *)elem.payload = value; }
            else { *(uint64_t *)elem.payload = value; }
            break;
        case Float: *(float *)elem.payload = value; break;
        case Double: *(double *)elem.payload = value; break;
        case Unknown: assert(false); break;
        }
    }
}

std::string String::GetValue(const Any &elem) const
{
    if (elem.typeinfo->GetKind() == Type::Kind::String)
    {
        return *AnyCast<std::string>(elem);
    }
    return nullptr;
}

void String::SetValue(const std::string &value, const Any &elem) const
{
    if (elem.typeinfo->GetKind() == Type::Kind::String)
    {
        *AnyCast<std::string>(elem) = value;
    }
}

Class &Class::AddProperty(const std::shared_ptr<Property> &prop)
{
    properties.emplace_back(prop);
    return *this;
}

}