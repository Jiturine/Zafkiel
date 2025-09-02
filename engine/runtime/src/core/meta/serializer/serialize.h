#pragma once
#include "core/meta/reflection/refl.h"
#include <yaml-cpp/yaml.h>
namespace Zafkiel
{
using namespace Reflection;

static void SerializeClass(std::any instance, const Class *typeInfo, YAML::Emitter &out)
{
    for (auto prop : typeInfo->GetProperties())
    {
        auto propTypeInfo = prop->GetTypeInfo();
        if (propTypeInfo->GetKind() == Type::Kind::Class)
        {
            auto classTypeInfo = propTypeInfo->As<Class>();
            std::any subInstance = prop->CallConst(instance);
            SerializeClass(subInstance, classTypeInfo, out);
        }
        else if (propTypeInfo->GetKind() == Type::Kind::Numeric)
        {
            auto numericTypeInfo = propTypeInfo->As<Numeric>();
            std::any anyOfNum = prop->CallConst(instance);
            if (numericTypeInfo->GetKind() == Numeric::Kind::Float)
            {
                float num = RemoveConstRef<float>(anyOfNum);
                out << YAML::Key << prop->GetName();
                out << YAML::Value << num;
            }
        }
    }
}

template <typename T>
std::string Serialize(const T &obj)
{
    const Type *typeInfo = GetType<T>();
    if (!typeInfo)
    {
        Log::CoreError("No reflection data!");
    }
    YAML::Emitter out;
    out << YAML::BeginMap;
    if (typeInfo->GetKind() == Type::Kind::Class)
    {
        auto classTypeInfo = typeInfo->As<Class>();
        std::any instance = std::cref(obj);
        SerializeClass(instance, classTypeInfo, out);
    }
    out << YAML::EndMap;

    return out.c_str();
}

}