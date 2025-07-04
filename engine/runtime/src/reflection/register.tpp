#pragma once
#include "register.h"
#include "property_traits.h"
#include <iostream>

namespace Zafkiel::Reflection
{

template <typename T>
TypeInfo<T, Numeric> &TypeInfo<T, Numeric>::Register(const std::string &name)
{
    info.name = name;
    info.kind = Numeric::DetectKind<T>();
    info.isSigned = std::is_signed_v<T>;
    if (!saved)
    {
        typeDict[info.name] = &info;
        saved = true;
    }
    return *this;
}

// 由于基础类型其实可以不用注册，所以在GetInfo时检测如果没注册过，会调用这个函数自动注册

template <typename T>
void TypeInfo<T, Numeric>::AutoRegister()
{
    info.kind = Numeric::DetectKind<T>();
    info.name = Numeric::GetNameOfKind(info.kind);
    info.isSigned = std::is_signed_v<T>;
    typeDict[info.name] = &info;
}

template <typename T>
TypeInfo<T, String> &TypeInfo<T, String>::Register(const std::string &name)
{
    if (!saved)
    {
        typeDict[info.name] = &info;
        saved = true;
    }
    info.name = name;
    return *this;
}

template <typename T>
void TypeInfo<T, String>::AutoRegister()
{
    info.name = "std::string";
    typeDict[info.name] = &info;
}

template <typename T>
TypeInfo<T, Enum> &TypeInfo<T, Enum>::Register(const std::string &name)
{
    info.name = name;
    if (!saved)
    {
        typeDict[info.name] = &info;
        saved = true;
    }
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
    info.name = name;
    if (!saved)
    {
        typeDict[info.name] = &info;
        saved = true;
    }
    return *this;
}

// 核心的GetType函数,后面属性的具体实现要用到

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

template <typename Ptr>
Property_Impl<Ptr>::Property_Impl(const std::string &name, const Class *owner, Ptr accessor)
    : Property(name, owner), accessor(accessor),
      info(GetType<typename property_traits<Ptr>::ValueType>())
{
}

template <typename T>
TypeInfo<T, Property> &TypeInfo<T, Property>::Register(const std::string &name, T accessor)
{
    using ClassType = property_traits<T>::ClassType;
    info = std::make_shared<Property_Impl<T>>(name, &TypeInfo<ClassType, Class>::Instance().GetInfo(), accessor);
    return *this;
}

// 最后实现类添加属性的方法，在内部就完成对属性子类型的注册

template <typename T>
template <typename Ptr>
TypeInfo<T, Class> &TypeInfo<T, Class>::AddProperty(Ptr accessor, const std::string &name)
{
    info.AddProperty(TypeInfo<Ptr, Property>::Instance().Register(name, accessor).GetInfo());
    return *this;
}

// 简化用户接口

template <typename T>
std::vector<std::pair<std::any, std::shared_ptr<Property>>> GetProperties(T &obj)
{
    const Class *type = GetType<T>()->template As<Class>();
    std::any a = std::ref(obj); // 通过std::ref使std::any存储引用
    std::vector<std::pair<std::any, std::shared_ptr<Property>>> ret;
    for (const auto &prop : type->GetProperties())
    {
        std::any val_ptr = prop->Call(a);
        ret.emplace_back(val_ptr, prop);
    }
    return ret;
}

template <typename T>
T &RemoveRef(std::any obj)
{
    return std::any_cast<std::reference_wrapper<T>>(obj).get();
}

template <typename T> bool TypeInfo<T, Class>::saved = false;
template <typename T> bool TypeInfo<T, String>::saved = false;
template <typename T> bool TypeInfo<T, Numeric>::saved = false;
template <typename T> bool TypeInfo<T, Enum>::saved = false;
}
