#pragma once

#include "core/meta/reflection/refl.h"
#include "core/meta/serializer/custom_serialize.h"

namespace Zafkiel
{

class [[refl]] UUID
{
  public:
    UUID();
    UUID(uint64_t);
    UUID(const UUID &) = default;
    operator uint64_t() const;
    std::string ToString() { return std::to_string(uuid); }

  private:
    uint64_t uuid;
};

template <>
struct Serialization<UUID>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const Any &instance, const Type *typeInfo, YAML::Emitter &out)
    {
        auto &uuidInstance = instance.As<UUID>();
        out << static_cast<uint64_t>(uuidInstance);
    }
    static void Deserialize(Any &instance, const Type *typeInfo, const YAML::Node &data)
    {
        auto &uuidInstance = instance.As<UUID>();
        uuidInstance = UUID(data.as<uint64_t>());
    }
};

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