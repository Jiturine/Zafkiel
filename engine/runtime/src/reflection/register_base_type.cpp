#include "register_base_type.h"

namespace Zafkiel::Reflection
{

// 通过类型名字来获取类型信息
const Type *GetType(const std::string &name)
{
    for (auto typeinfo : typeList)
    {
        if (typeinfo->GetName() == name) return typeinfo;
    }
    return nullptr;
}
}