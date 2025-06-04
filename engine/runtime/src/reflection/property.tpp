#pragma once

#include "property.h"

namespace Zafkiel::Reflection
{
template <typename Ptr>
NumericProperty_Impl<Ptr>::NumericProperty_Impl(const std::string &name, const Class *owner, Ptr pointer)
    : NumericProperty(name, owner), pointer(pointer),
      info(GetType<typename variable_traits<Ptr>::type>()->template As<Numeric>())
{
}

template <typename Ptr>
StringProperty_Impl<Ptr>::StringProperty_Impl(const std::string &name, const Class *owner, Ptr pointer)
    : StringProperty(name, owner), pointer(pointer),
      info(GetType<typename variable_traits<Ptr>::type>()->template As<String>())
{
    std::cout << info->GetName() << std::endl;
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

}