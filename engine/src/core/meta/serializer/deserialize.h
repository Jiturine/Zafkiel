#pragma once

#include "core/meta/reflection/refl.h"
#include "custom_serialize.h"
#include <yaml-cpp/yaml.h>

namespace Zafkiel
{
using namespace Reflection;

static void DeserializeAny(Any instance, const Type *typeInfo, const YAML::Node &data, Any context = nullptr);

static void DeserializeFundamental(Any instance, const Fundamental *typeInfo, const YAML::Node &data)
{
    switch (typeInfo->GetKind())
    {
        using enum FundamentalKind;
    case Bool: instance.As<bool>() = data.as<bool>(); break;
    case Char: instance.As<char>() = data.as<char>(); break;
    case Int8: instance.As<int8_t>() = data.as<int8_t>(); break;
    case Int16: instance.As<int16_t>() = data.as<int16_t>(); break;
    case Int32: instance.As<int32_t>() = data.as<int32_t>(); break;
    case Int64: instance.As<int64_t>() = data.as<int64_t>(); break;
    case UInt8: instance.As<uint8_t>() = data.as<uint8_t>(); break;
    case UInt16: instance.As<uint16_t>() = data.as<uint16_t>(); break;
    case UInt32: instance.As<uint32_t>() = data.as<uint32_t>(); break;
    case UInt64: instance.As<uint64_t>() = data.as<uint64_t>(); break;
    case Float: instance.As<float>() = data.as<float>(); break;
    case Double: instance.As<double>() = data.as<double>(); break;
    default: break;
    }
}

static void DeserializeString(Any instance, const String *typeInfo, const YAML::Node &data)
{
    const auto &str = data.as<std::string>();
    instance.As<std::string>() = str;
}

static void DeserializeEnum(Any instance, const Enum *typeInfo, const YAML::Node &data)
{
    typeInfo->SetValueName(instance, data.as<std::string>());
}

static void DeserializeList(Any instance, const List *typeInfo, const YAML::Node &data)
{
    const auto *elemTypeInfo = typeInfo->GetElemType();
    typeInfo->Resize(data.size(), instance); // 重要
    for (size_t i = 0; i < data.size(); i++)
    {
        Any elem = typeInfo->GetElem(i, instance);
        DeserializeAny(elem, elemTypeInfo, data[i]);
    }
}

static void DeserializeDict(Any instance, const Dict *typeInfo, const YAML::Node &data)
{
    const auto *keyTypeInfo = typeInfo->GetKeyType();
    const auto *valTypeInfo = typeInfo->GetValType();

    for (const auto &kvp : data)
    {
        Any key = keyTypeInfo->CreateInstance();
        Any val = valTypeInfo->CreateInstance();
        DeserializeAny(key, keyTypeInfo, kvp.first);
        DeserializeAny(val, valTypeInfo, kvp.second);
        typeInfo->Insert(instance, key, val);
    }
}

static void DeserializeProperty(Any instance, const std::shared_ptr<Property> &prop, const YAML::Node &data)
{
    Any subInstance = prop->Call(instance);
    DeserializeAny(subInstance, prop->GetTypeInfo(), data);
}

static void DeserializeClass(Any instance, const Class *typeInfo, const YAML::Node &data)
{
    const auto &props = typeInfo->GetProperties();
    for (const auto &prop : props)
    {
        DeserializeProperty(instance, prop, data[prop->GetName()]);
    }
}

static void DeserializeAny(Any instance, const Type *typeInfo, const YAML::Node &data, Any context)
{
    if (auto it = customSerializeOps.find(typeInfo); it != customSerializeOps.end())
    {
        it->second.deserializeFunc(instance, context, data);
        return;
    }
    switch (typeInfo->GetCategory())
    {
    case TypeCategory::Fundamental: DeserializeFundamental(instance, typeInfo->As<Fundamental>(), data); break;
    case TypeCategory::String: DeserializeString(instance, typeInfo->As<String>(), data); break;
    case TypeCategory::Enum: DeserializeEnum(instance, typeInfo->As<Enum>(), data); break;
    case TypeCategory::List: DeserializeList(instance, typeInfo->As<List>(), data); break;
    case TypeCategory::Dict: DeserializeDict(instance, typeInfo->As<Dict>(), data); break;
    case TypeCategory::Class: DeserializeClass(instance, typeInfo->As<Class>(), data); break;
    default: Log::CoreError("Unknown Type!"); break;
    }
}

template <typename T>
T Deserialize(const std::string &str)
{
    T obj;
    const YAML::Node data = YAML::Load(str);
    const Type *typeInfo = GetType<T>();
    if (!typeInfo)
    {
        Log::CoreError("No reflection data!");
    }
    Any instance = obj;
    DeserializeAny(instance, typeInfo, data);

    return obj;
}

template <typename T>
void Deserialize(const std::string &str, T &emptyObj)
{
    const YAML::Node data = YAML::Load(str);
    const Type *typeInfo = GetType<T>();
    if (!typeInfo)
    {
        Log::CoreError("No reflection data!");
    }
    Any instance = emptyObj;
    DeserializeAny(instance, typeInfo, data);
}
}