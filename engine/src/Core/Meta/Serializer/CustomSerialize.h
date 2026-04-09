#pragma once

#include "Core/Meta/Reflection/Type.h"
#include <yaml-cpp/yaml.h>
#include "Core/Meta/Serializer/SerializerBase.h"

namespace Zafkiel
{
using namespace Reflection;

struct SerializeOperations
{
    std::function<void(const AnyRef, AnyRef, ISerializer &)> serializeFunc;
    std::function<void(AnyRef, AnyRef, IDeserializer &)> deserializeFunc;
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
