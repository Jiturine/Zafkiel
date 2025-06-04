#pragma once

#include "base_type.h"

namespace Zafkiel::Reflection
{

// 内部通过类成员指针，从类的Any中访问属性的Any
template <typename Ptr>
Any CallProperty(Any &a, Ptr accessor, const Type *owner);

template <typename Ptr>
class NumericProperty_Impl : public NumericProperty
{
  public:
    NumericProperty_Impl(const std::string &name, const Class *owner, Ptr pointer);

    Any Call(Any &a) const override { return CallProperty(a, pointer, GetOwner()); }

    virtual const Type *GetTypeInfo() const override { return info; };

  private:
    Ptr pointer = nullptr;
    const Numeric *info;
};

template <typename Ptr>
class StringProperty_Impl : public StringProperty
{
  public:
    StringProperty_Impl(const std::string &name, const Class *owner, Ptr pointer);

    Any Call(Any &a) const override { return CallProperty(a, pointer, GetOwner()); }

    virtual const Type *GetTypeInfo() const override { return info; };

  private:
    Ptr pointer = nullptr;
    const String *info;
};

template <typename Ptr>
class EnumProperty_Impl : public EnumProperty
{
  public:
    EnumProperty_Impl(const std::string &name, const Class *owner, Ptr pointer);

    Any Call(Any &a) const override { return CallProperty(a, pointer, GetOwner()); }

    virtual const Type *GetTypeInfo() const override { return info; };

  private:
    Ptr pointer = nullptr;
    const Enum *info;
};

template <typename Ptr>
class ClassProperty_Impl : public ClassProperty
{
  public:
    ClassProperty_Impl(const std::string &name, const Class *owner, Ptr pointer);

    Any Call(Any &a) const override { return CallProperty(a, pointer, GetOwner()); }

    virtual const Type *GetTypeInfo() const override { return info; };

  private:
    Ptr pointer = nullptr;
    const Class *info;
};

}

#include "property.tpp"