#pragma once
#include "any.h"
#include "type_traits.h"

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
    std::function<int(const Any &)> getValueFunc;
    std::function<void(Any &, int)> setValueFunc;
};

template <typename ElemType>
struct EnumOperations
{
    static int GetValue(const Any &instance)
    {
        return static_cast<int>(instance.As<ElemType>());
    }
    static void SetValue(Any &instance, int value)
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

    int GetValue(const Any &) const;

    std::string GetValueName(const Any &) const;

    void SetValue(Any &, int value) const;

    void SetValueName(Any &, const std::string &itemName) const;

    virtual Any CreateInstance() const override { return createInstanceFunc(); }
  private:
    std::vector<Item> items;
    EnumFunctions enumFunctions;
    CreateInstanceFunc createInstanceFunc;
};

struct ListFunctions
{
    std::function<Any(size_t, Any &)> getElemFunc;
    std::function<const Any(size_t, const Any &)> getElemConstFunc;
    std::function<Any(Any &)> getBackFunc;
    std::function<const Any(const Any &)> getBackConstFunc;
    std::function<size_t(const Any &)> getSizeFunc;
    std::function<void(size_t, Any &)> resizeFunc;
};

template <typename ElemType>
struct ListOperations
{
    static Any GetElem(size_t index, Any &instance)
    {
        auto &lst = instance.As<std::vector<ElemType>>();
        return lst[index];
    }
    static const Any GetElemConst(size_t index, const Any &instance)
    {
        auto &lst = instance.As<std::vector<ElemType>>();
        return lst[index];
    }
    static Any GetBack(Any &instance)
    {
        auto &lst = instance.As<std::vector<ElemType>>();
        return lst.back();
    }
    static const Any GetBackConst(const Any &instance)
    {
        auto &lst = instance.As<std::vector<ElemType>>();
        return lst.back();
    }
    static size_t GetSize(const Any &instance)
    {
        auto &lst = instance.As<std::vector<ElemType>>();
        return lst.size();
    }
    static void Resize(size_t size, Any &instance)
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

    Any GetElem(size_t index, Any &) const;
    const Any GetElem(size_t index, const Any &) const;
    Any GetBack(Any &) const;
    const Any GetBack(const Any &) const;
    size_t GetSize(const Any &) const;
    void Resize(size_t, Any &) const;
    virtual Any CreateInstance() const override { return createInstanceFunc(); }

  private:
    const Type *elemType;
    const ListFunctions listFunctions;
    CreateInstanceFunc createInstanceFunc;
};

struct DictFunctions
{
    std::function<std::vector<std::pair<Any, Any>>(const Any &)> getKeyValPairsFunc;
    std::function<void(Any &, const Any &, const Any &)> insertFunc;
    std::function<size_t(const Any &)> getSizeFunc;
};

template <typename KeyType, typename ValType>
struct DictOperations
{
    static std::vector<std::pair<Any, Any>> GetKeyValPairs(const Any &instance)
    {
        auto &dict = instance.As<std::unordered_map<KeyType, ValType>>();
        std::vector<std::pair<Any, Any>> res;
        for (const auto &[k, v] : dict)
        {
            Any instance_k = k;
            Any instance_v = v;
            res.emplace_back(instance_k, instance_v);
        }
        return res;
    }

    static void Insert(Any &instance, const Any &key, const Any &val)
    {
        auto &dict = instance.As<std::unordered_map<KeyType, ValType>>();

        auto &keyInstance = key.As<KeyType>();
        auto &valInstance = val.As<ValType>();

        dict.insert({std::move(keyInstance), std::move(valInstance)});
    }

    static size_t GetSize(const Any &instance)
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
    std::vector<std::pair<Any, Any>> GetKeyValPairs(const Any &instance) const;
    void Insert(Any &instance, const Any &key, const Any &val) const;
    size_t GetSize(const Any &instance) const;
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
        Any ins = createInstanceFunc();
        return ins;
    }


  private:
    std::vector<std::shared_ptr<Property>> properties;
    CreateInstanceFunc createInstanceFunc;
};

class Property : public Type
{
  public:
    Property(const std::string &name, const Class *owner)
        : Type(name, TypeCategory::Property), owner(owner) {}

    ~Property() = default;

    virtual Any Call(Any &) const = 0;
    virtual Any Call(const Any &) const = 0;
    virtual const Type *GetTypeInfo() const = 0;

    const Class *GetOwner() const { return owner; }
  private:
    const Class *owner;
    const Type *info;
};

template <typename T>
struct CreateInstanceOperations
{
    static Any CreateInstance() { return std::make_unique<T>(); }
};

}

#include "type.tpp"