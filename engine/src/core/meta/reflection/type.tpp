#pragma once
#include "type.h"

namespace Zafkiel::Reflection
{
template <typename T>
    requires std::derived_from<T, Type>
const T *Type::As() const
{
    return static_cast<const T *>(this);
}

Enum &Enum::Add(auto value, const std::string &name)
{
    items.emplace_back(name, static_cast<int>(value));
    return *this;
}

}
