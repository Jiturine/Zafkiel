#pragma once
#include "make_any.h"
#include "register.h"

namespace Zafkiel::Reflection
{

template <typename T>
Any MakeCopy(const T &elem)
{
    Any ret_value;
    ret_value.payload = new T{elem};
    ret_value.typeinfo = GetType<T>();
    ret_value.storageType = Any::StorageType::Copy;
    if constexpr (std::is_copy_constructible_v<T>) { ret_value.ops.copy = &operations_traits<T>::Copy; }
    if constexpr (std::is_move_constructible_v<T>) { ret_value.ops.move = &operations_traits<T>::Move; }
    if constexpr (std::is_destructible_v<T>) { ret_value.ops.release = &operations_traits<T>::Release; }
    return ret_value;
}

template <typename T>
Any MakeMove(T &&elem)
{
    Any ret_value;
    ret_value.payload = new std::remove_reference_t<T>{std::move(elem)};
    ret_value.typeinfo = GetType<T>();
    ret_value.storageType = Any::StorageType::Copy;
    if constexpr (std::is_copy_constructible_v<T>) { ret_value.ops.copy = &operations_traits<T>::Copy; }
    if constexpr (std::is_move_constructible_v<T>) { ret_value.ops.move = &operations_traits<T>::Move; }
    if constexpr (std::is_destructible_v<T>) { ret_value.ops.release = &operations_traits<T>::Release; }
    return ret_value;
}

template <typename T>
Any MakeRef(const T &elem)
{
    Any ret_value;
    ret_value.payload = &elem;
    ret_value.typeinfo = GetType<T>();
    ret_value.storageType = Any::StorageType::Ref;
    if constexpr (std::is_copy_constructible_v<T>) { ret_value.ops.copy = &operations_traits<T>::Copy; }
    if constexpr (std::is_move_constructible_v<T>) { ret_value.ops.move = &operations_traits<T>::Move; }
    if constexpr (std::is_destructible_v<T>) { ret_value.ops.release = &operations_traits<T>::Release; }
    return ret_value;
}

template <typename T>
Any MakeConstRef(const T &elem)
{
    Any ret_value;
    ret_value.payload = const_cast<T *>(&elem);
    ret_value.typeinfo = GetType<T>();
    ret_value.storageType = Any::StorageType::ConstRef;
    if constexpr (std::is_copy_constructible_v<T>) { ret_value.ops.copy = &operations_traits<T>::Copy; }
    if constexpr (std::is_move_constructible_v<T>) { ret_value.ops.move = &operations_traits<T>::Move; }
    if constexpr (std::is_destructible_v<T>) { ret_value.ops.release = &operations_traits<T>::Release; }
    return ret_value;
}

template <typename T>
T *AnyCast(const Any &elem)
{
    if (elem.typeinfo == GetType<T>()) { return static_cast<T *>(elem.payload); }
    else return nullptr;
}

template <class T>
std::vector<std::pair<std::shared_ptr<Property>, Any>> GetPropertiesOfInstance(const T &instance)
{
    const Class *typeOfInstance = GetType<T>()->template As<Class>();
    Any anyOfInstance = MakeConstRef(instance);
    std::vector<std::pair<std::shared_ptr<Property>, Any>> propsAndAny;
    for (const auto &prop : typeOfInstance->GetProperties())
    {
        Any propsAsAny = prop->Call(anyOfInstance);
        propsAndAny.emplace_back(prop, propsAsAny);
    }
    return propsAndAny;
}
}
