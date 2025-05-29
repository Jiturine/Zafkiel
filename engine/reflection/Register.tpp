#pragma once
#include <type_traits>
#include "Register.h"
#include "variable_traits.h"

namespace Reflection
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
TypeInfo<T, Enum> &TypeInfo<T, Enum>::Add(const std::string &name, auto value)
{
    info.Add(name, value);
    return *this;
}

template <typename T>
TypeInfo<T, NumericProperty> &TypeInfo<T, NumericProperty>::Register(const std::string &name, T accessor)
{
    using ClassType = variable_traits<T>::ClassType;
    info = std::make_shared<NumericProperty_Impl<T>>(name, &TypeInfo<ClassType, Class>::Instance().GetInfo(), accessor);
    return *this;
}

template <typename T>
TypeInfo<T, EnumProperty> &TypeInfo<T, EnumProperty>::Register(const std::string &name, T accessor)
{
    using ClassType = variable_traits<T>::ClassType;
    info = std::make_shared<EnumProperty_Impl<T>>(name, &TypeInfo<ClassType, Class>::Instance().GetInfo(), accessor);
    return *this;
}

template <typename T>
TypeInfo<T, ClassProperty> &TypeInfo<T, ClassProperty>::Register(const std::string &name, T accessor)
{
    using ClassType = variable_traits<T>::ClassType;
    info = std::make_shared<ClassProperty_Impl<T>>(name, &TypeInfo<ClassType, Class>::Instance().GetInfo(), accessor);
    return *this;
}

// 核心的GetType函数，这里没有属性的概念，因为属性自己有对应类型
// 后面要通过GetType来获取属性对应的类型

template <typename T>
const Type *GetType()
{
    if constexpr (std::is_fundamental_v<T>) { return &TypeInfo<T, Numeric>::Instance().GetInfo(); }
    else if constexpr (std::is_enum_v<T>) { return &TypeInfo<T, Enum>::Instance().GetInfo(); }
    else if constexpr (std::is_class_v<T>) { return &TypeInfo<T, Class>::Instance().GetInfo(); }
    else return nullptr;
}

template <typename Ptr>
NumericProperty_Impl<Ptr>::NumericProperty_Impl(const std::string &name, const Class *owner, Ptr pointer)
    : NumericProperty(name, owner), pointer(pointer),
      info(GetType<typename variable_traits<Ptr>::type>()->template As<Numeric>())
{
}

template <typename Ptr>
EnumProperty_Impl<Ptr>::EnumProperty_Impl(const std::string &name, const Class *owner, Ptr pointer)
    : EnumProperty(name, owner), pointer(pointer),
      info(GetType<typename variable_traits<Ptr>::type>()->template As<Enum>())
{
}

template <typename Ptr>
ClassProperty_Impl<Ptr>::ClassProperty_Impl(const std::string &name, const Class *owner, Ptr pointer)
    : ClassProperty(name, owner), pointer(pointer),
      info(GetType<typename variable_traits<Ptr>::type>()->template As<Class>())
{
}

template <typename Ptr>
Any CallProperty(Any &a, Ptr accessor, const Type *owner)
{
    if (owner != a.typeinfo || a.storageType == Any::StorageType::Empty) { return {}; }
    using traits = variable_traits<Ptr>;
    using ClassType = typename traits::ClassType;
    using type = traits::type;

    // 通过accessor类成员指针来访问成员
    auto &value = ((ClassType *)a.payload)->*accessor;
    auto &operations = operations_traits<type>::GetOperations();
    auto info = GetType<type>();

    return Any(info, (void *)&value, Any::StorageType::Ref, operations);
}

// 类类型添加属性:通过类型分类，先注册属性类型，然后将类型信息存入类的TypeInfo的properties列表

template <typename T>
template <typename Ptr>
TypeInfo<T, Class> &TypeInfo<T, Class>::AddProperty(Ptr accessor, const std::string &name)
{
    using traits = variable_traits<Ptr>;
    using type = typename traits::type;
    if constexpr (std::is_fundamental_v<type>)
    {
        info.AddProperty(TypeInfo<Ptr, NumericProperty>::Instance().Register(name, accessor).GetInfo());
    }
    else if constexpr (std::is_enum_v<type>)
    {
        info.AddProperty(TypeInfo<Ptr, EnumProperty>::Instance().Register(name, accessor).GetInfo());
    }
    else if constexpr (std::is_class_v<type>)
    {
        info.AddProperty(TypeInfo<Ptr, ClassProperty>::Instance().Register(name, accessor).GetInfo());
    }
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

// 核心的Register，根据不同类型，进行不同TypeInfo的注册
template <typename T>
auto &Register(const std::string &name)
{
    if constexpr (std::is_fundamental_v<T>) { return TypeInfo<T, Numeric>::Instance().Register(name); }
    else if constexpr (std::is_enum_v<T>) { return TypeInfo<T, Enum>::Instance().Register(name); }
    else if constexpr (std::is_class_v<T>) { return TypeInfo<T, Class>::Instance().Register(name); }
}

template <typename T> bool TypeInfo<T, Class>::saved = false;
template <typename T> bool TypeInfo<T, Numeric>::saved = false;
template <typename T> bool TypeInfo<T, Enum>::saved = false;
}