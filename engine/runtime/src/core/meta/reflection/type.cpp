#include "type.h"

namespace Zafkiel::Reflection
{
Fundamental::Fundamental(const std::string &name, FundamentalKind kind)
    : Type(name, TypeCategory::Fundamental), kind(kind)
{
    typeDict[name] = this;
}

String::String() : Type("std::string", TypeCategory::String)
{
    typeDict[name] = this;
}

Enum::Enum(const EnumFunctions &enumFunctions)
    : Type(TypeCategory::Enum), enumFunctions(enumFunctions) {}

List::List(const Type *elemType, const ListFunctions &listFuncs)
    : Type(std::format("std::vector<{}>", elemType->GetName()), TypeCategory::List),
      elemType(elemType), listFunctions(listFuncs)
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