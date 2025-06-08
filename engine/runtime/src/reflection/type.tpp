#pragma once
#include "type.h"

namespace Zafkiel::Reflection
{
template <typename T>
    requires std::derived_from<T, Type>
const T *Type::As() const
{
    if (kind == DetectKind<T>()) return dynamic_cast<const T *>(this);
    else return nullptr;
}

template <typename T>
    requires std::derived_from<T, Type>
Type::Kind Type::DetectKind()
{
    if constexpr (std::is_same_v<T, Numeric>) return Kind::Numeric;
    else if constexpr (std::is_same_v<T, String>) return Kind::String;
    else if constexpr (std::is_same_v<T, Enum>) return Kind::Enum;
    else if constexpr (std::is_same_v<T, Class>) return Kind::Class;
    else return Kind::Unknown;
}

template <typename T>
    requires std::is_fundamental_v<T>
Numeric::Kind Numeric::DetectKind()
{
    if constexpr (std::is_same_v<T, int8_t>) return Kind::Int8;
    else if constexpr (std::is_same_v<T, int16_t>) return Kind::Int16;
    else if constexpr (std::is_same_v<T, int32_t>) return Kind::Int32;
    else if constexpr (std::is_same_v<T, int64_t>) return Kind::Int64;
    else if constexpr (std::is_same_v<T, float>) return Kind::Float;
    else if constexpr (std::is_same_v<T, double>) return Kind::Double;
    else return Kind::Unknown;
}

template <typename T>
Enum &Enum::Add(const std::string &name, T value)
{
    items.emplace_back(name, static_cast<Item::ValueType>(value));
    return *this;
}

}
