#include "register.h"

namespace Zafkiel::Reflection
{
// 通过类型名字来获取类型信息
const Type *GetType(const std::string &name)
{
    if (typeDict.contains(name))
    {
        return typeDict[name];
    }
    return nullptr;
}

std::vector<std::pair<std::any, std::shared_ptr<Property>>> GetProperties(const Class *type, std::any obj)
{
    std::vector<std::pair<std::any, std::shared_ptr<Property>>> ret;
    for (const auto &prop : type->GetProperties())
    {
        std::any val_ptr = prop->Call(obj);
        ret.emplace_back(val_ptr, prop);
    }
    return ret;
}
}