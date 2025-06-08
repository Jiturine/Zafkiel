#pragma once
#include "variable_traits.h"

namespace Zafkiel::Reflection
{

class Type
{
  public:
    template <typename, typename>
    friend class TypeInfo;

    enum class Kind
    {
        Unknown,
        Numeric, // 数字类型
        String,  // 字符串类型
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
  private:
    Kind kind;
    bool isSigned;

    template <typename T>
        requires std::is_fundamental_v<T>
    static Kind DetectKind();
};

// 字符串类型
class String : public Type
{
  public:
    String() : Type(Type::Kind::String) {}
    String(const std::string &name) : Type(name, Type::Kind::String) {}
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

    const std::vector<Item> &GetItems() const { return items; }

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

    const std::vector<std::shared_ptr<Property>> &GetProperties() const { return properties; }

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

    virtual std::any Call(const std::any &) const = 0;
    virtual const Type *GetTypeInfo() const = 0;

    const Class *GetOwner() const { return owner; }
  private:
    const Class *owner;
};

}

#include "type.tpp"