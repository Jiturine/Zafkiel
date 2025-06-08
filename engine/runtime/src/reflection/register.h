#pragma once
#include "type.h"
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

// 核心的GetType函数

template <typename T>
const Type *GetType();

// 核心的Register，根据不同类型，进行不同TypeInfo的注册
template <typename T>
auto &Register(const std::string &name);

// Ptr 为类成员指针类型

template <typename Ptr>
class Property_Impl : public Property
{
  public:
    Property_Impl(const std::string &name, const Class *owner, Ptr pointer);

    std::any Call(const std::any &a) const override
    {
        using traits = variable_traits<Ptr>;
        using ClassType = traits::ClassType;
        using type = traits::type;
        if (GetType<ClassType>() != GetOwner())
            throw std::runtime_error("Type mismatch");
        auto &obj = std::any_cast<std::reference_wrapper<ClassType>>(a).get(); //所有的std::any存储的是引用，这里要解除std::ref的引用
        std::any result = std::ref(obj.*pointer);
        return result;
    }

    virtual const Type *GetTypeInfo() const override { return info; };

  private:
    Ptr pointer = nullptr;
    const Type *info;
};

template <typename T>
class TypeInfo<T, Property> : public Singleton<TypeInfo<T, Property>>
{
  public:
    TypeInfo &Register(const std::string &name, T accessor);

    std::shared_ptr<Property> GetInfo() const { return info; }
  private:
    std::shared_ptr<Property> info;
};

template <typename T>
std::vector<std::pair<std::any, std::shared_ptr<Property>>> GetProperties(T &obj);

std::vector<std::pair<std::any, std::shared_ptr<Property>>> GetProperties(const Class *type, std::any obj);

template <typename T>
T &RemoveRef(std::any obj);

};

#include "register.tpp"