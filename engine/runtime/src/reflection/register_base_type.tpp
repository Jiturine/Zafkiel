#pragma once
#include <type_traits>
#include "register_base_type.h"
#include "variable_traits.h"

#include <iostream>

namespace Zafkiel::Reflection
{

template <typename T>
TypeInfo<T, Numeric> &TypeInfo<T, Numeric>::Register(const std::string &name)
{
    if (!saved)
    {
        typeList.push_back(&info);
        saved = true;
    }
    info.name = name;
    info.kind = Numeric::DetectKind<T>();
    info.isSigned = std::is_signed_v<T>;
    return *this;
}

// 由于基础类型其实可以不用注册，所以在GetInfo时检测如果没注册过，会调用这个函数自动注册

template <typename T>
void TypeInfo<T, Numeric>::AutoRegister()
{
    if (!saved)
    {
        typeList.push_back(&info);
        saved = true;
    }
    info.kind = Numeric::DetectKind<T>();
    info.name = Numeric::GetNameOfKind(info.kind);
    info.isSigned = std::is_signed_v<T>;
}

template <typename T>
TypeInfo<T, String> &TypeInfo<T, String>::Register(const std::string &name)
{
    if (!saved)
    {
        typeList.push_back(&info);
        saved = true;
    }
    info.name = name;
    return *this;
}

template <typename T>
void TypeInfo<T, String>::AutoRegister()
{
    if (!saved)
    {
        typeList.push_back(&info);
        saved = true;
    }
    info.name = "std::string";
}

template <typename T>
TypeInfo<T, Enum> &TypeInfo<T, Enum>::Register(const std::string &name)
{
    if (!saved)
    {
        typeList.push_back(&info);
        saved = true;
    }
    info.name = name;
    return *this;
}

template <typename T>
TypeInfo<T, Enum> &TypeInfo<T, Enum>::Add(auto value, const std::string &name)
{
    info.Add(name, value);
    return *this;
}

template <typename T>
TypeInfo<T, Class> &TypeInfo<T, Class>::Register(const std::string &name)
{
    if (!saved)
    {
        typeList.push_back(&info);
        saved = true;
    }
    info.name = name;
    return *this;
}

// 核心的GetType函数，这里没有属性的概念，因为属性自己有对应类型
// 后面要通过GetType来获取属性对应的类型

template <typename T>
const Type *GetType()
{
    if constexpr (std::is_fundamental_v<T>) { return &TypeInfo<T, Numeric>::Instance().GetInfo(); }
    else if constexpr (std::is_same_v<T, std::string>)
    {
        return &TypeInfo<T, String>::Instance().GetInfo();
    }
    else if constexpr (std::is_enum_v<T>) { return &TypeInfo<T, Enum>::Instance().GetInfo(); }
    else if constexpr (std::is_class_v<T>) { return &TypeInfo<T, Class>::Instance().GetInfo(); }
    else return nullptr;
}

template <typename T>
auto &Register(const std::string &name)
{
    if constexpr (std::is_fundamental_v<T>) { return TypeInfo<T, Numeric>::Instance().Register(name); }
    else if constexpr (std::is_same_v<T, std::string>) { return TypeInfo<T, String>::Instance().Register(name); }
    else if constexpr (std::is_enum_v<T>) { return TypeInfo<T, Enum>::Instance().Register(name); }
    else if constexpr (std::is_class_v<T>) { return TypeInfo<T, Class>::Instance().Register(name); }
}

template <typename T> bool TypeInfo<T, Class>::saved = false;
template <typename T> bool TypeInfo<T, String>::saved = false;
template <typename T> bool TypeInfo<T, Numeric>::saved = false;
template <typename T> bool TypeInfo<T, Enum>::saved = false;
}
