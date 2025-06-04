#pragma once
#include "any.h"

namespace Zafkiel::Reflection
{
template <typename T>
Any operations_traits<T>::Copy(const Any &elem)
{
    Any ret_value;
    ret_value.payload = new std::remove_reference_t<T>{*static_cast<std::remove_reference_t<T> *>(elem.payload)};
    ret_value.typeinfo = elem.typeinfo;
    ret_value.storageType = Any::StorageType::Copy;
    ret_value.ops = elem.ops;
    return ret_value;
}

template <typename T>
Any operations_traits<T>::Move(Any &&elem)
{
    Any ret_value;
    ret_value.payload =
        new std::remove_reference_t<T>{std::move(*static_cast<std::remove_reference_t<T> *>(elem.payload))};
    ret_value.typeinfo = elem.typeinfo;
    ret_value.storageType = Any::StorageType::Copy;
    elem.storageType = Any::StorageType::Empty;
    ret_value.ops = elem.ops;
    return ret_value;
}

template <typename T>
void operations_traits<T>::Release(Any &elem)
{
    delete static_cast<std::remove_reference_t<T> *>(elem.payload);
    elem.payload = nullptr;
    elem.storageType = Any::StorageType::Empty;
    elem.typeinfo = nullptr;
}

template <typename T>
const Any::Operations &operations_traits<T>::GetOperations()
{
    using traits = operations_traits<T>;
    static Any::Operations operations = {traits::Copy, traits::Move, traits::Release};
    return operations;
}
}
