#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Any.h"

namespace Reflection
{
class Any;

class Type
{
  public:
    template <typename T, typename TypeKind>
    friend class TypeInfo;

    enum class Kind
    {
        Unknown,
        Numeric, // 数字类型
        Enum,    // 枚举类型
        Class,   // 类类型
        Property // 属性(类成员)类型
    };
    virtual ~Type() = default;
    Type(Kind kind) : kind(kind) {}
    Type(const std::string &name, Kind kind) : name(name), kind(kind) {}

    // 将基类Type转换为对应子类
    template <typename T>
        requires std::derived_from<T, Type>
    const T *As() const;

    std::string GetName() const { return name; }
    Kind GetKind() const { return kind; }

  private:
    template <typename T>
        requires std::derived_from<T, Type>
    static Kind DetectKind();

    std::string name;
    Kind kind;
};

// 数字类型
class Numeric : public Type
{
  public:
    template <typename T, typename TypeKind>
    friend class TypeInfo;

    enum class Kind
    {
        Unknown,
        Int8,
        Int16,
        Int32,
        Int64,
        Float,
        Double
    };
    Kind GetKind() const { return kind; }
    bool IsSigned() const { return isSigned; }
    static std::string GetNameOfKind(Kind kind);

    Numeric() : Type(Type::Kind::Numeric) {}
    Numeric(Kind kind, bool isSigned) : Type(GetNameOfKind(kind), Type::Kind::Numeric), kind(kind), isSigned(isSigned)
    {
    }

    void SetValue(int64_t value, Any &elem) const;
  private:
    Kind kind;
    bool isSigned;

    template <typename T>
        requires std::is_fundamental_v<T>
    static Kind DetectKind();
};

// 枚举类型
class Enum : public Type
{
  public:
    struct Item
    {
        using ValueType = int;
        std::string name;
        ValueType value;
    };

    Enum() : Type(Type::Kind::Enum) {}
    Enum(const std::string &name) : Type(name, Type::Kind::Enum) {}

    std::vector<Item> GetItems() const { return items; }

    template <typename T>
    Enum &Add(const std::string &name, T value);

  private:
    std::vector<Item> items;
};

class Property;

// 类类型
class Class : public Type
{
  public:
    Class() : Type(Type::Kind::Class) {}
    Class(const std::string &name) : Type(name, Type::Kind::Class) {}

    std::vector<std::shared_ptr<Property>> GetProperties() const { return properties; }

    Class &AddProperty(const std::shared_ptr<Property> &prop);

  private:
    std::vector<std::shared_ptr<Property>> properties;
};

// 属性类型在所有其他类型声明之后写，免于前置声明

class Property : public Type
{
  public:
    Property() : Type(Type::Kind::Property) {}
    Property(const std::string &name, const Class *owner) : Type(name, Type::Kind::Property), owner(owner) {}

    ~Property() = default;

    virtual Any Call(Any &) const = 0;
    virtual const Type *GetTypeInfo() const = 0;

    const Class *GetOwner() const { return owner; }
  private:
    const Class *owner;
};

//每个类型的属性对应存储的基类，之后的具体实现是模板，没法存，所以必须存基类

class NumericProperty : public Property
{
    using Property::Property;
};

class EnumProperty : public Property
{
    using Property::Property;
};

class ClassProperty : public Property
{
    using Property::Property;
};

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

#include "Type.tpp"