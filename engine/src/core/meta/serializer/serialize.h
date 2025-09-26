#pragma once
#include "core/meta/reflection/refl.h"
#include "custom_serialize.h"
#include <yaml-cpp/yaml.h>

namespace Zafkiel
{
using namespace Reflection;

static void SerializeAny(const Any instance, const Type *typeInfo, YAML::Emitter &out, Any context = nullptr);

static void SerializeFundamental(const Any instance, const Fundamental *typeInfo, YAML::Emitter &out)
{
    switch (typeInfo->GetKind())
    {
        using enum FundamentalKind;
    case Bool: out << instance.As<bool>(); break;
    case Char: out << instance.As<char>(); break;
    case Int8: out << instance.As<int8_t>(); break;
    case Int16: out << instance.As<int16_t>(); break;
    case Int32: out << instance.As<int32_t>(); break;
    case Int64: out << instance.As<int64_t>(); break;
    case UInt8: out << instance.As<uint8_t>(); break;
    case UInt16: out << instance.As<uint16_t>(); break;
    case UInt32: out << instance.As<uint32_t>(); break;
    case UInt64: out << instance.As<uint64_t>(); break;
    case Float: out << instance.As<float>(); break;
    case Double: out << instance.As<double>(); break;
    default: break;
    }
}

static void SerializeString(const Any instance, const String *typeInfo, YAML::Emitter &out)
{
    out << instance.As<std::string>();
}

static void SerializeEnum(const Any instance, const Enum *typeInfo, YAML::Emitter &out)
{
    out << typeInfo->GetValueName(instance);
}

static void SerializeList(const Any instance, const List *typeInfo, YAML::Emitter &out)
{
    out << YAML::BeginSeq;
    for (size_t i = 0; i < typeInfo->GetSize(instance); i++)
    {
        const Any elem = typeInfo->GetElem(i, instance);
        SerializeAny(elem, typeInfo->GetElemType(), out);
    }
    out << YAML::EndSeq;
}

static void SerializeDict(const Any instance, const Dict *typeInfo, YAML::Emitter &out)
{
    out << YAML::BeginMap;
    for (const auto &[key, val] : typeInfo->GetKeyValPairs(instance))
    {
        out << YAML::Key;
        SerializeAny(key, typeInfo->GetKeyType(), out);
        out << YAML::Value;
        SerializeAny(val, typeInfo->GetValType(), out);
    }
    out << YAML::EndMap;
}

static void SerializeProperty(const Any instance, const std::shared_ptr<Property> prop, YAML::Emitter &out)
{
    auto propTypeInfo = prop->GetTypeInfo();
    out << YAML::Key << prop->GetName() << YAML::Value;
    SerializeAny(prop->Call(instance), propTypeInfo, out);
}

static void SerializeClass(const Any instance, const Class *typeInfo, YAML::Emitter &out)
{
    out << YAML::BeginMap;
    for (auto prop : typeInfo->GetProperties())
    {
        SerializeProperty(instance, prop, out);
    }
    out << YAML::EndMap;
}

static void SerializeAny(const Any instance, const Type *typeInfo, YAML::Emitter &out, Any context)
{
    if (auto it = customSerializeOps.find(typeInfo); it != customSerializeOps.end())
    {
        it->second.serializeFunc(instance, context, out);
        return;
    }
    switch (typeInfo->GetCategory())
    {
    case TypeCategory::Fundamental: SerializeFundamental(instance, typeInfo->As<Fundamental>(), out); break;
    case TypeCategory::String: SerializeString(instance, typeInfo->As<String>(), out); break;
    case TypeCategory::Enum: SerializeEnum(instance, typeInfo->As<Enum>(), out); break;
    case TypeCategory::List: SerializeList(instance, typeInfo->As<List>(), out); break;
    case TypeCategory::Dict: SerializeDict(instance, typeInfo->As<Dict>(), out); break;
    case TypeCategory::Class: SerializeClass(instance, typeInfo->As<Class>(), out); break;
    default: Log::CoreError("Unknown Type!"); break;
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

    const Any instance = obj;
    SerializeAny(instance, typeInfo, out);

    return out.c_str();
}

}