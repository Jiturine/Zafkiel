#pragma once

namespace Reflection
{
// 类型萃取相关

namespace detail
{

template <typename T>
struct variable_type
{
    using type = T;
};

template <typename Class, typename T>
struct variable_type<T Class::*>
{
    using type = T;
};

}
template <typename T> using variable_type_t = typename detail::variable_type<T>::type;

template <typename T>
struct basic_variable_traits
{
    using type = variable_type_t<T>;
    static constexpr bool is_member = std::is_member_pointer_v<T>;
};

template <typename T>
struct variable_traits;

template <typename T>
struct variable_traits<T *> : basic_variable_traits<T>
{
    using pointer = T *;
};

// 萃取得到类成员指针的类类型和成员类型
template <typename Class, typename T>
struct variable_traits<T Class::*> : basic_variable_traits<T Class::*>
{
    using pointer = T Class::*;
    using ClassType = Class;
};

}
