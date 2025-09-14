#include "type.h"

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
    case Bool: return std::make_unique<bool>(); break;
    case Char: return std::make_unique<char>(); break;
    case Int8: return std::make_unique<int8_t>(); break;
    case Int16: return std::make_unique<int16_t>(); break;
    case Int32: return std::make_unique<int32_t>(); break;
    case Int64: return std::make_unique<int64_t>(); break;
    case UInt8: return std::make_unique<uint8_t>(); break;
    case UInt16: return std::make_unique<uint16_t>(); break;
    case UInt32: return std::make_unique<uint32_t>(); break;
    case UInt64: return std::make_unique<uint64_t>(); break;
    case Float: return std::make_unique<float>(); break;
    case Double: return std::make_unique<double>(); break;
    default: return nullptr; break;
    }
}

String::String() : Type("std::string", TypeCategory::String)
{
    typeDict[name] = this;
}

Any String::CreateInstance() const
{
    return std::make_unique<std::string>();
}

Enum::Enum(const EnumFunctions &enumFunctions, CreateInstanceFunc func)
    : Type(TypeCategory::Enum), enumFunctions(enumFunctions), createInstanceFunc(func) {}

List::List(const Type *elemType, const ListFunctions &listFuncs, CreateInstanceFunc func)
    : Type(std::format("std::vector<{}>", elemType->GetName()), TypeCategory::List),
      elemType(elemType), listFunctions(listFuncs), createInstanceFunc(func)
{
    typeDict[name] = this;
}

Any List::GetElem(size_t index, Any &instance) const
{
    return listFunctions.getElemFunc(index, instance);
}
const Any List::GetElem(size_t index, const Any &instance) const
{
    return listFunctions.getElemConstFunc(index, instance);
}
Any List::GetBack(Any &instance) const
{
    return listFunctions.getBackFunc(instance);
}
const Any List::GetBack(const Any &instance) const
{
    return listFunctions.getBackConstFunc(instance);
}
size_t List::GetSize(const Any &instance) const
{
    return listFunctions.getSizeFunc(instance);
}
void List::Resize(size_t size, Any &instance) const
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

std::vector<std::pair<Any, Any>> Dict::GetKeyValPairs(const Any &instance) const
{
    return dictFunctions.getKeyValPairsFunc(instance);
}
void Dict::Insert(Any &instance, const Any &key, const Any &val) const
{
    dictFunctions.insertFunc(instance, key, val);
}
size_t Dict::GetSize(const Any &instance) const
{
    return dictFunctions.getSizeFunc(instance);
}

int Enum::GetValue(const Any &instance) const
{
    return enumFunctions.getValueFunc(instance);
}

std::string Enum::GetValueName(const Any &instance) const
{
    int val = GetValue(instance);
    for (const auto &item : items)
    {
        if (item.value == val)
        {
            return item.name;
        }
    }
    return nullptr;
}

void Enum::SetValue(Any &instance, int value) const
{
    enumFunctions.setValueFunc(instance, value);
}

void Enum::SetValueName(Any &instance, const std::string &itemName) const
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