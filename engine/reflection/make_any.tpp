#pragma once
#include "make_any.h"

namespace Reflection
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
Any MakeRef(T &elem)
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
    ret_value.payload = &elem;
    ret_value.typeinfo = GetType<T>();
    ret_value.storageType = Any::StorageType::ConstRef;
    if constexpr (std::is_copy_constructible_v<T>) { ret_value.ops.copy = &operations_traits<T>::Copy; }
    if constexpr (std::is_move_constructible_v<T>) { ret_value.ops.move = &operations_traits<T>::Move; }
    if constexpr (std::is_destructible_v<T>) { ret_value.ops.release = &operations_traits<T>::Release; }
    return ret_value;
}

template <typename T>
T *try_cast(Any &elem)
{
    if (elem.typeinfo == GetType<T>()) { return (T *)elem.payload; }
    else return nullptr;
}

}