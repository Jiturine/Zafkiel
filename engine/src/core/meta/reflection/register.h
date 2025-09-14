#pragma once
#include "type.h"
#include "core/base/singleton.h"

namespace Zafkiel::Reflection
{

template <typename T>
class FundamentalTypeInfo
{
  public:
    static Fundamental &GetInfo()
    {
        static Fundamental info(fundamental_traits<T>::name, fundamental_traits<T>::kind);
        return info;
    }
};

template <typename T>
class StringTypeInfo
{
  public:
    static String &GetInfo()
    {
        static String info;
        return info;
    }
};

template <typename T>
class EnumTypeInfo
{
  public:
    static Enum &Register(const std::string &name);
    static Enum &GetInfo()
    {
        static Enum info({EnumOperations<T>::GetValue, EnumOperations<T>::SetValue}, CreateInstanceOperations<T>::CreateInstance);
        return info;
    }
};

template <typename T>
class ListTypeInfo
{
  public:
    static List &GetInfo();
};

template <typename T>
class DictTypeInfo
{
  public:
    static Dict &GetInfo();
};

// 存储类类型信息的TypeInfo
template <typename T>
class ClassTypeInfo : public Singleton<ClassTypeInfo<T>>
{
  public:
    static ClassTypeInfo &Register(const std::string &name);

    template <typename Ptr>
    static ClassTypeInfo &AddProperty(Ptr accessor, const std::string &name);

    static Class &GetInfo()
    {
        static Class info(CreateInstanceOperations<T>::CreateInstance);
        return info;
    }
};

// 核心的GetType函数

template <typename T>
const Type *GetType();

const Type *GetType(const std::string &name);

// 核心的Register，根据不同类型，进行不同TypeInfo的注册
template <typename T>
auto &Register(const std::string &name);

template <typename T>
ListFunctions GetListFunctions()
{
    using ListOps = ListOperations<T>;
    return ListFunctions{ListOps::GetElem, ListOps::GetElemConst, ListOps::GetBack, ListOps::GetBackConst, ListOps::GetSize, ListOps::Resize};
}

template <typename T>
List &ListTypeInfo<T>::GetInfo()
{
    static List info(GetType<typename list_traits<T>::ElemType>(), GetListFunctions<typename list_traits<T>::ElemType>(),
        CreateInstanceOperations<T>::CreateInstance);
    return info;
}

template <typename Key, typename Val>
DictFunctions GetDictFunctions()
{
    using DictOps = DictOperations<Key, Val>;
    return DictFunctions{DictOps::GetKeyValPairs, DictOps::Insert, DictOps::GetSize};
}

template <typename T>
Dict &DictTypeInfo<T>::GetInfo()
{
    static Dict info(
        GetType<typename dict_traits<T>::KeyType>(), GetType<typename dict_traits<T>::ValType>(),
        GetDictFunctions<typename dict_traits<T>::KeyType, typename dict_traits<T>::ValType>(),
        CreateInstanceOperations<T>::CreateInstance);
    return info;
}
// Ptr 为类成员指针类型

template <typename Ptr>
class Property_Impl : public Property
{
  public:
    Property_Impl(const std::string &name, const Class *owner, Ptr accessor);

    Any Call(Any &instance) const override;
    Any Call(const Any &instance) const override;

    virtual const Type *GetTypeInfo() const override { return info; };

  private:
    Ptr accessor = nullptr;
    const Type *info;
};

template <typename T>
class PropertyTypeInfo
{
  public:
    static PropertyTypeInfo &Register(const std::string &name, T accessor);

    static std::shared_ptr<Property> GetInfo() { return info; }
  private:
    static std::shared_ptr<Property> info;
};

template <typename T>
std::shared_ptr<Property> PropertyTypeInfo<T>::info;

template <typename T>
std::vector<std::pair<Any, std::shared_ptr<Property>>> GetProperties(T &obj);

template <typename T>
std::vector<std::pair<Any, std::shared_ptr<Property>>> GetProperties(const T &obj);

template <typename Ptr>
Ptr GetPropertyAddress(Ptr accessor)
{
    return accessor;
}

};

#include "register.tpp"