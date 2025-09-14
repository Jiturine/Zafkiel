#pragma once

#include "../reflection/type.h"
#include <yaml-cpp/yaml.h>

namespace Zafkiel
{
using namespace Reflection;

struct SerializeOperations
{
    std::function<void(const Any &, const Type *, YAML::Emitter &)> serializeFunc;
    std::function<void(Any &, const Type *, const YAML::Node &)> deserializeFunc;
};

template <typename T>
struct Serialization
{
    static constexpr bool has_serialize = false;
};

inline std::unordered_map<const Type *, SerializeOperations> customSerializeOps;

template <typename T>
void TryRegisterSerializer()
{
    if constexpr (Serialization<T>::has_serialize)
    {
        customSerializeOps[GetType<T>()] = {Serialization<T>::Serialize, Serialization<T>::Deserialize};
    }
}

}
