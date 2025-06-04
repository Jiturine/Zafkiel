#pragma once
#include "property.h"

namespace Zafkiel::Reflection
{

// 存储各个属性类型信息的TypeInfo

template <typename T>
class TypeInfo<T, NumericProperty> : public Singleton<TypeInfo<T, NumericProperty>>
{
  public:
    TypeInfo &Register(const std::string &name, T accessor);

    std::shared_ptr<Property> GetInfo() const { return info; }
  private:
    std::shared_ptr<NumericProperty> info;
};

template <typename T>
class TypeInfo<T, StringProperty> : public Singleton<TypeInfo<T, StringProperty>>
{
  public:
    TypeInfo &Register(const std::string &name, T accessor);

    std::shared_ptr<Property> GetInfo() const { return info; }
  private:
    std::shared_ptr<StringProperty> info;
};

template <typename T>
class TypeInfo<T, EnumProperty> : public Singleton<TypeInfo<T, EnumProperty>>
{
  public:
    TypeInfo &Register(const std::string &name, T accessor);

    std::shared_ptr<Property> GetInfo() const { return info; }
  private:
    std::shared_ptr<EnumProperty> info;
};

template <typename T>
class TypeInfo<T, ClassProperty> : public Singleton<TypeInfo<T, ClassProperty>>
{
  public:
    TypeInfo &Register(const std::string &name, T accessor);

    std::shared_ptr<Property> GetInfo() const { return info; }
  private:
    std::shared_ptr<ClassProperty> info;
};

template <typename Ptr>
Any CallProperty(Any &a, Ptr accessor, const Type *owner);
}

#include "register_property.tpp"