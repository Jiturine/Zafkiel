#pragma once

namespace Zafkiel::Reflection
{
template <typename T>
TypeInfo<T, NumericProperty> &TypeInfo<T, NumericProperty>::Register(const std::string &name, T accessor)
{
    using ClassType = variable_traits<T>::ClassType;
    info = std::make_shared<NumericProperty_Impl<T>>(name, &TypeInfo<ClassType, Class>::Instance().GetInfo(), accessor);
    return *this;
}

template <typename T>
TypeInfo<T, StringProperty> &TypeInfo<T, StringProperty>::Register(const std::string &name, T accessor)
{
    using ClassType = variable_traits<T>::ClassType;
    info = std::make_shared<StringProperty_Impl<T>>(name, &TypeInfo<ClassType, Class>::Instance().GetInfo(), accessor);
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
    using rawtype = variable_traits<Ptr>::type;
    using type = std::decay_t<rawtype>;
    if constexpr (std::is_fundamental_v<type>)
    {
        info.AddProperty(TypeInfo<Ptr, NumericProperty>::Instance().Register(name, accessor).GetInfo());
    }
    else if constexpr (std::is_same_v<type, std::string>)
    {
        info.AddProperty(TypeInfo<Ptr, StringProperty>::Instance().Register(name, accessor).GetInfo());
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

}