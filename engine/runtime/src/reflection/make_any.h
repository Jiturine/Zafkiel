#pragma once
#include "any.h"

namespace Zafkiel::Reflection
{

// 这几个MakeXXX是给用户的接口，用于将具体类型的对象转换成Any类型，
// 便于后面通过Type信息来访问对象的属性

// 通过拷贝的方式创建Any对象
template <typename T>
Any MakeCopy(const T &elem);

// 通过移动的方式创建Any对象
template <typename T>
Any MakeMove(T &&elem);

// 通过引用的方式创建Any对象
template <typename T>
Any MakeRef(const T &elem);

// 通过常引用的方式创建Any对象
template <typename T>
Any MakeConstRef(const T &elem);

// 尝试将Any转换成要求的具体类型
template <typename T>
T *AnyCast(const Any &elem);

template <class T>
std::vector<std::pair<std::shared_ptr<Property>, Any>> GetPropertiesOfInstance(const T &instance);
}

#include "make_any.tpp"