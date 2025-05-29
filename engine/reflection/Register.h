#pragma once
#include <list>
#include "Type.h"
#include "../utils/Singleton.h"

namespace Reflection
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
    friend const Type *GetType<T>();

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

// 存储枚举类型信息的TypeInfo
template <typename T>
class TypeInfo<T, Enum> : public Singleton<TypeInfo<T, Enum>>
{
  public:
    TypeInfo &Register(const std::string &name);
    TypeInfo &Add(const std::string &name, auto value);

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

// 后面是存储各个属性类型信息的TypeInfo

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

};

#include "Register.tpp"