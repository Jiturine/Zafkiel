#include "core/meta/reflection/type.h"

namespace Zafkiel::Reflection
{
Fundamental::Fundamental(const std::string &name, FundamentalKind kind)
    : Type(name, TypeCategory::Fundamental), kind(kind)
{
    typeDict[name] = this;
}

Any Fundamental::CreateInstance() const
{
    switch (kind)
    {
        using enum FundamentalKind;
    case Bool: return bool{}; break;
    case Char: return char{}; break;
    case Int8: return int8_t{}; break;
    case Int16: return int16_t{}; break;
    case Int32: return int32_t{}; break;
    case Int64: return int64_t{}; break;
    case UInt8: return uint8_t{}; break;
    case UInt16: return uint16_t{}; break;
    case UInt32: return uint32_t{}; break;
    case UInt64: return uint64_t{}; break;
    case Float: return float{}; break;
    case Double: return double{}; break;
    default: return nullptr; break;
    }
}

String::String() : Type("std::string", TypeCategory::String)
{
    typeDict[name] = this;
}

Any String::CreateInstance() const
{
    return std::string{};
}

Enum::Enum(const EnumFunctions &enumFunctions, CreateInstanceFunc func)
    : Type(TypeCategory::Enum), enumFunctions(enumFunctions), createInstanceFunc(func) {}

List::List(const Type *elemType, const ListFunctions &listFuncs, CreateInstanceFunc func)
    : Type(std::format("std::vector<{}>", elemType->GetName()), TypeCategory::List),
      elemType(elemType), listFunctions(listFuncs), createInstanceFunc(func)
{
    typeDict[name] = this;
}

AnyRef List::GetElem(size_t index, AnyRef &instance) const
{
    return listFunctions.getElemFunc(index, instance);
}
const AnyRef List::GetElem(size_t index, const AnyRef &instance) const
{
    return listFunctions.getElemConstFunc(index, instance);
}
AnyRef List::GetBack(AnyRef &instance) const
{
    return listFunctions.getBackFunc(instance);
}
const AnyRef List::GetBack(const AnyRef &instance) const
{
    return listFunctions.getBackConstFunc(instance);
}
size_t List::GetSize(const AnyRef &instance) const
{
    return listFunctions.getSizeFunc(instance);
}
void List::Resize(size_t size, AnyRef &instance) const
{
    listFunctions.resizeFunc(size, instance);
}

Dict::Dict(const Type *keyType, const Type *valType, const DictFunctions &dictFuncs, CreateInstanceFunc func)
    : Type(std::format("std::unordered_map<{}, {}>", keyType->GetName(), valType->GetName()),
          TypeCategory::Dict),
      keyType(keyType), valType(valType), dictFunctions(dictFuncs), createInstanceFunc(func)
{
    typeDict[name] = this;
}

std::vector<std::pair<AnyRef, AnyRef>> Dict::GetKeyValPairs(const AnyRef &instance) const
{
    return dictFunctions.getKeyValPairsFunc(instance);
}
void Dict::Insert(AnyRef &instance, const AnyRef &key, const AnyRef &val) const
{
    dictFunctions.insertFunc(instance, key, val);
}
size_t Dict::GetSize(const AnyRef &instance) const
{
    return dictFunctions.getSizeFunc(instance);
}

int Enum::GetValue(const AnyRef &instance) const
{
    return enumFunctions.getValueFunc(instance);
}

std::string Enum::GetValueName(const AnyRef &instance) const
{
    int val = GetValue(instance);
    for (const auto &item : items)
    {
        if (item.value == val)
        {
            return item.name;
        }
    }
    return {};
}

void Enum::SetValue(AnyRef &instance, int value) const
{
    enumFunctions.setValueFunc(instance, value);
}

void Enum::SetValueName(AnyRef &instance, const std::string &itemName) const
{
    for (const auto &item : items)
    {
        if (item.name == itemName)
        {
            SetValue(instance, item.value);
            break;
        }
    }
}

Class &Class::AddProperty(const std::shared_ptr<Property> &prop)
{
    properties.emplace_back(prop);
    return *this;
}

}