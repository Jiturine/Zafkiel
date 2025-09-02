#pragma once

namespace Zafkiel::Reflection
{

template <typename>
struct is_array : std::false_type
{
};
template <typename T, typename Alloc>
struct is_array<std::vector<T, Alloc>> : std::true_type
{
};
template <typename T>
inline constexpr bool is_array_v = is_array<T>::value;

template <typename>
struct array_traits;

template <typename T, typename Alloc>
struct array_traits<std::vector<T, Alloc>>
{
    using ElemType = T;
};

template <typename>
struct property_traits;

template <typename Class_, typename Value_>
struct property_traits<Value_ Class_::*>
{
    using ClassType = Class_;
    using ValueType = Value_;
};

}
