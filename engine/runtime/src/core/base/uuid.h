#pragma once

namespace Zafkiel
{
class UUID
{
  public:
    UUID();
    UUID(uint64_t uuid);
    UUID(const UUID &other) = default;

    operator uint64_t() const { return uuid; }

  private:
    uint64_t uuid;
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