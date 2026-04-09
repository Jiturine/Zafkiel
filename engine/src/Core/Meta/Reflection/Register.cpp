#include "Core/Meta/Reflection/Register.h"

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

}