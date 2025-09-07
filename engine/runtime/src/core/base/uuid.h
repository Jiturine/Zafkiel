#pragma once

#include "core/meta/reflection/refl.h"
#include "core/meta/serializer/custom_serialize.h"

namespace Zafkiel
{
class [[refl]] UUID
{
  private:
    uint64_t [[norefl]] uuid;
  public:
    UUID();
    UUID(uint64_t uuid);
    UUID(const UUID &other) = default;

    operator uint64_t() const
    {
        return uuid;
    }
    static void Serialize(const Any &instance, const Type *typeInfo, YAML::Emitter &out)
    {
        auto uuidInstance = instance.As<UUID>();
        out << static_cast<uint64_t>(uuidInstance);
    }
    static void Deserialize(Any &instance, const Type *typeInfo, const YAML::Node &data)
    {
        auto uuidInstance = instance.As<UUID>();
        uuidInstance = UUID(data.as<uint64_t>());
    }
};

static CustomSerialize<UUID> customSerializeUUID({UUID::Serialize, UUID::Deserialize});

}

namespace std
{
// 哈希函数
template <>
struct hash<Zafkiel::UUID>
{
    std::size_t operator()(const Zafkiel::UUID &uuid) const
    {
        return hash<uint64_t>()((uint64_t)uuid);
    }
};
}