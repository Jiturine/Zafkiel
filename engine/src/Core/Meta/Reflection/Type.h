#pragma once
#include "Core/Meta/Reflection/Any.h"
#include "Core/Meta/Reflection/TypeTraits.h"

namespace Zafkiel::Reflection
{

using CreateInstanceFunc = std::function<Any(void)>;

class Type
{
  public:
    template <typename>
    friend class EnumTypeInfo;
    template <typename>
    friend class ClassTypeInfo;

    virtual ~Type() = default;

    Type(TypeCategory category) : category(category) {}
    Type(const std::string &name, TypeCategory category) : name(name), category(category) {}

    // 将基类Type转换为对应子类
    template <typename T>
        requires std::derived_from<T, Type>
    const T *As() const;

    std::string GetName() const { return name; }
    TypeCategory GetCategory() const { return category; }
    virtual Any CreateInstance() const { return nullptr; }

  protected:
    std::string name;
    TypeCategory category;
};

// 存储所有已注册的类型的类列表，便于只通过类型名string获取类型信息
inline std::unordered_map<std::string, const Type *> typeDict;

class Fundamental : public Type
{
  public:
    Fundamental(const std::string &name, FundamentalKind kind);

    FundamentalKind GetKind() const { return kind; }

    virtual Any CreateInstance() const override;
  private:
    FundamentalKind kind;
};

class String : public Type
{
  public:
    String();
    virtual Any CreateInstance() const override;
};

struct EnumFunctions
{
    std::function<int(const AnyRef &)> getValueFunc;
    std::function<void(AnyRef &, int)> setValueFunc;
};

template <typename ElemType>
struct EnumOperations
{
    static int GetValue(const AnyRef &instance)
    {
        return static_cast<int>(instance.As<ElemType>());
    }
    static void SetValue(AnyRef &instance, int value)
    {
        instance.As<ElemType>() = static_cast<ElemType>(value);
    }
};

class Enum : public Type
{
  public:
    template <typename>
    friend class EnumTypeInfo;

    Enum(const EnumFunctions &enumFunctions, CreateInstanceFunc func);

    struct Item
    {
        std::string name;
        int value;
    };

    const std::vector<Item> &GetItems() const { return items; }

    Enum &Add(auto value, const std::string &name);

    int GetValue(const AnyRef &) const;

    std::string GetValueName(const AnyRef &) const;

    void SetValue(AnyRef &, int value) const;

    void SetValueName(AnyRef &, const std::string &itemName) const;

    virtual Any CreateInstance() const override { return createInstanceFunc(); }
  private:
    std::vector<Item> items;
    EnumFunctions enumFunctions;
    CreateInstanceFunc createInstanceFunc;
};

struct ListFunctions
{
    std::function<AnyRef(size_t, AnyRef &)> getElemFunc;
    std::function<const AnyRef(size_t, const AnyRef &)> getElemConstFunc;
    std::function<AnyRef(AnyRef &)> getBackFunc;
    std::function<const AnyRef(const AnyRef &)> getBackConstFunc;
    std::function<size_t(const AnyRef &)> getSizeFunc;
    std::function<void(size_t, AnyRef &)> resizeFunc;
};

template <typename ElemType>
struct ListOperations
{
    static AnyRef GetElem(size_t index, AnyRef &instance)
    {
        auto &lst = instance.As<std::vector<ElemType>>();
        return lst[index];
    }
    static const AnyRef GetElemConst(size_t index, const AnyRef &instance)
    {
        auto &lst = instance.As<std::vector<ElemType>>();
        return lst[index];
    }
    static AnyRef GetBack(AnyRef &instance)
    {
        auto &lst = instance.As<std::vector<ElemType>>();
        return lst.back();
    }
    static const AnyRef GetBackConst(const AnyRef &instance)
    {
        auto &lst = instance.As<std::vector<ElemType>>();
        return lst.back();
    }
    static size_t GetSize(const AnyRef &instance)
    {
        auto &lst = instance.As<std::vector<ElemType>>();
        return lst.size();
    }
    static void Resize(size_t size, AnyRef &instance)
    {
        auto &lst = instance.As<std::vector<ElemType>>();
        lst.resize(size);
    }
};

class List : public Type
{
  public:
    List(const Type *elemType, const ListFunctions &listFuncs, CreateInstanceFunc func);

    const Type *GetElemType() const { return elemType; }

    AnyRef GetElem(size_t index, AnyRef &) const;
    const AnyRef GetElem(size_t index, const AnyRef &) const;
    AnyRef GetBack(AnyRef &) const;
    const AnyRef GetBack(const AnyRef &) const;
    size_t GetSize(const AnyRef &) const;
    void Resize(size_t, AnyRef &) const;
    virtual Any CreateInstance() const override { return createInstanceFunc(); }

  private:
    const Type *elemType;
    const ListFunctions listFunctions;
    CreateInstanceFunc createInstanceFunc;
};

struct DictFunctions
{
    std::function<std::vector<std::pair<AnyRef, AnyRef>>(const AnyRef &)> getKeyValPairsFunc;
    std::function<void(AnyRef &, const AnyRef &, const AnyRef &)> insertFunc;
    std::function<size_t(const AnyRef &)> getSizeFunc;
};

template <typename KeyType, typename ValType>
struct DictOperations
{
    static std::vector<std::pair<AnyRef, AnyRef>> GetKeyValPairs(const AnyRef &instance)
    {
        auto &dict = instance.As<std::unordered_map<KeyType, ValType>>();
        std::vector<std::pair<AnyRef, AnyRef>> res;
        for (const auto &[k, v] : dict)
        {
            AnyRef instance_k = k;
            AnyRef instance_v = v;
            res.emplace_back(instance_k, instance_v);
        }
        return res;
    }

    static void Insert(AnyRef &instance, const AnyRef &key, const AnyRef &val)
    {
        auto &dict = instance.As<std::unordered_map<KeyType, ValType>>();

        auto &keyInstance = key.As<KeyType>();
        auto &valInstance = val.As<ValType>();

        dict.insert({keyInstance, valInstance});
    }

    static size_t GetSize(const AnyRef &instance)
    {
        auto &dict = instance.As<std::unordered_map<KeyType, ValType>>();
        return dict.size();
    }
};

class Dict : public Type
{
  public:
    Dict(const Type *keyType, const Type *valType, const DictFunctions &dictFuncs, CreateInstanceFunc func);

    const Type *GetKeyType() const { return keyType; }
    const Type *GetValType() const { return valType; }
    std::vector<std::pair<AnyRef, AnyRef>> GetKeyValPairs(const AnyRef &instance) const;
    void Insert(AnyRef &instance, const AnyRef &key, const AnyRef &val) const;
    size_t GetSize(const AnyRef &instance) const;
    virtual Any CreateInstance() const override { return createInstanceFunc(); }

  private:
    const Type *keyType;
    const Type *valType;
    const DictFunctions dictFunctions;
    CreateInstanceFunc createInstanceFunc;
};

class Property;

// 类类型
class Class : public Type
{
  public:
    Class(CreateInstanceFunc func) : Type(TypeCategory::Class), createInstanceFunc(func) {}

    const std::vector<std::shared_ptr<Property>> &GetProperties() const { return properties; }

    Class &AddProperty(const std::shared_ptr<Property> &prop);
    virtual Any CreateInstance() const override
    {
        return createInstanceFunc();
    }

  private:
    std::vector<std::shared_ptr<Property>> properties;
    CreateInstanceFunc createInstanceFunc;
};

class Property
{
  public:
    Property(const std::string &name, const Class *owner)
        : name(name), owner(owner) {}

    virtual AnyRef Call(AnyRef &) const = 0;
    virtual AnyRef Call(const AnyRef &) const = 0;
    virtual const Type *GetTypeInfo() const = 0;

    const Class *GetOwner() const { return owner; }
    std::string GetName() const { return name; }

  private:
    const Class *owner;
    std::string name;
};

template <typename T>
struct CreateInstanceOperations
{
    static Any CreateInstance() { return T{}; }
};

}

#include "Core/Meta/Reflection/Type.tpp"