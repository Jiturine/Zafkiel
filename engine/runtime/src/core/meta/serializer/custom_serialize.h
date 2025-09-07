#pragma once

#include "core/meta/reflection/type.h"
#include <yaml-cpp/yaml.h>

namespace Zafkiel
{
using namespace Reflection;

struct SerializeOperations
{
    std::function<void(const Any &, const Type *, YAML::Emitter &)> serializeFunc;
    std::function<void(Any &, const Type *, const YAML::Node &)> deserializeFunc;
};

inline std::unordered_map<const Type *, SerializeOperations> customSerializeOps;

template <typename T>
struct CustomSerialize
{
    CustomSerialize(SerializeOperations ops)
    {
        customSerializeOps[GetType<T>()] = ops;
    }
};

}
