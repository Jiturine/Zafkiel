#pragma once
#include "base_type.h"
#include "../utils/singleton.h"

namespace Zafkiel::Reflection
{

// 存储所有已注册的类型的类列表，便于只通过类型名string获取类型信息
inline std::list<const Type *> typeList;

// 所有TypeInfo类为单例，存储对应类型的唯一一份数据
template <typename T, typename Kind>
class TypeInfo : public Singleton<TypeInfo<T, Kind>>
{
};

// 后面的TypeInfo都是模板特化，所以这里也可以不用参数Kind，单独实现每种TypeInfo类也行
// 我这里觉得用模板比较符合直觉

// 存储数字类型信息的TypeInfo
template <typename T>
class TypeInfo<T, Numeric> : public Singleton<TypeInfo<T, Numeric>>
{
  public:

    TypeInfo &Register(const std::string &name);
    void AutoRegister();

    const Numeric &GetInfo() const
    {
        if (!TypeInfo<T, Numeric>::Instance().saved) { TypeInfo<T, Numeric>::Instance().AutoRegister(); }
        return info;
    }
  private:
    static bool saved;
    Numeric info;
};

template <typename T>
class TypeInfo<T, String> : public Singleton<TypeInfo<T, String>>
{
  public:
    TypeInfo &Register(const std::string &name);
    void AutoRegister();

    const String &GetInfo() const
    {
        if (!TypeInfo<T, String>::Instance().saved) { TypeInfo<T, String>::Instance().AutoRegister(); }
        return info;
    }
  private:
    static bool saved;
    String info;
};

// 存储枚举类型信息的TypeInfo
template <typename T>
class TypeInfo<T, Enum> : public Singleton<TypeInfo<T, Enum>>
{
  public:
    TypeInfo &Register(const std::string &name);
    TypeInfo &Add(auto value, const std::string &name);

    const Enum &GetInfo() const { return info; }
  private:
    static bool saved;
    Enum info;
};

// 存储类类型信息的TypeInfo
template <typename T>
class TypeInfo<T, Class> : public Singleton<TypeInfo<T, Class>>
{
  public:
    TypeInfo &Register(const std::string &name);
    template <typename Ptr> TypeInfo &AddProperty(Ptr accessor, const std::string &name);

    const Class &GetInfo() const { return info; }
  private:
    static bool saved;
    Class info;
};

// 核心的GetType函数，这里没有属性的概念，因为属性自己有对应类型
// 后面要通过GetType来获取属性对应的类型

template <typename T>
const Type *GetType();

// 核心的Register，根据不同类型，进行不同TypeInfo的注册
template <typename T>
auto &Register(const std::string &name);

};

#include "register_base_type.tpp"