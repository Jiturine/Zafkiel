#pragma once
#include "Register.h"
#include "TypeTraits.h"
#include "Core/Meta/Serializer/CustomSerialize.h"
#include <iostream>

namespace Zafkiel::Reflection
{

template <typename T>
Enum &EnumTypeInfo<T>::Register(const std::string &name)
{
    Enum &info = GetInfo();
    info.name = name;
    typeDict[info.name] = &info;
    return info;
}

template <typename T>
ClassTypeInfo<T> &ClassTypeInfo<T>::Register(const std::string &name)
{
    Class &info = GetInfo();
    info.name = name;
    typeDict[info.name] = &info;
    return Singleton<ClassTypeInfo<T>>::Instance();
}

// 核心的GetType函数,后面属性的具体实现要用到

template <typename T>
const Type *GetType()
{
    if constexpr (std::is_fundamental_v<T>) { return &FundamentalTypeInfo<T>::GetInfo(); }
    else if constexpr (std::is_same_v<T, std::string>) { return &StringTypeInfo<T>::GetInfo(); }
    else if constexpr (std::is_enum_v<T>) { return &EnumTypeInfo<T>::GetInfo(); }
    else if constexpr (is_list_v<T>) { return &ListTypeInfo<T>::GetInfo(); }
    else if constexpr (is_dict_v<T>) { return &DictTypeInfo<T>::GetInfo(); }
    else if constexpr (std::is_class_v<T>) { return &ClassTypeInfo<T>::GetInfo(); }
    else return nullptr;
}

template <typename T>
auto &Register(const std::string &name)
{
    TryRegisterSerializer<T>();
    if constexpr (std::is_enum_v<T>) { return EnumTypeInfo<T>::Register(name); }
    else if constexpr (std::is_class_v<T>) { return ClassTypeInfo<T>::Register(name); }
}

template <typename Ptr>
Property_Impl<Ptr>::Property_Impl(const std::string &name, const Class *owner, Ptr accessor)
    : Property(name, owner), accessor(accessor),
      info(GetType<typename property_traits<Ptr>::ValueType>())
{
}

template <typename Ptr>
AnyRef Property_Impl<Ptr>::Call(AnyRef &instance) const
{
    using ClassType = property_traits<Ptr>::ClassType;
    if (GetType<ClassType>() != GetOwner())
        throw std::runtime_error("Type mismatch");
    auto &obj = instance.As<ClassType>();
    auto &val = obj.*accessor;
    return val;
}

template <typename Ptr>
AnyRef Property_Impl<Ptr>::Call(const AnyRef &instance) const
{
    using ClassType = property_traits<Ptr>::ClassType;
    if (GetType<ClassType>() != GetOwner())
        throw std::runtime_error("Type mismatch");
    auto &obj = instance.As<ClassType>();
    const auto &val = obj.*accessor;
    return val;
}

template <typename T>
PropertyTypeInfo<T> &PropertyTypeInfo<T>::Register(const std::string &name, T accessor)
{
    using ClassType = property_traits<T>::ClassType;
    info = std::make_shared<Property_Impl<T>>(name, &ClassTypeInfo<ClassType>::GetInfo(), accessor);
    return Singleton<PropertyTypeInfo<T>>::Instance();
}

// 最后实现类添加属性的方法，在内部就完成对属性子类型的注册

template <typename T>
template <typename Ptr>
ClassTypeInfo<T> &ClassTypeInfo<T>::AddProperty(Ptr accessor, const std::string &name)
{
    Class &info = GetInfo();
    info.AddProperty(PropertyTypeInfo<Ptr>::Register(name, accessor).GetInfo());
    return Singleton<ClassTypeInfo<T>>::Instance();
}

// 简化用户接口

template <typename T>
std::vector<std::pair<AnyRef, std::shared_ptr<Property>>> GetProperties(T &obj)
{
    const Class *type = GetType<T>()->template As<Class>();
    AnyRef instance = obj;
    std::vector<std::pair<AnyRef, std::shared_ptr<Property>>> ret;
    for (const auto &prop : type->GetProperties())
    {
        AnyRef subInstance = prop->Call(instance);
        ret.emplace_back(subInstance, prop);
    }
    return ret;
}

template <typename T>
std::vector<std::pair<AnyRef, std::shared_ptr<Property>>> GetProperties(const T &obj)
{
    const Class *type = GetType<T>()->template As<Class>();
    const AnyRef instance = obj;
    std::vector<std::pair<AnyRef, std::shared_ptr<Property>>> ret;
    for (const auto &prop : type->GetProperties())
    {
        AnyRef subInstance = prop->Call(instance);
        ret.emplace_back(subInstance, prop);
    }
    return ret;
}

}
