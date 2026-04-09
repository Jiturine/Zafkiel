#include "Core/Base/UUID.h"

#include <random>

namespace Zafkiel
{
static std::random_device randomDevice;
static std::mt19937_64 engine(randomDevice());
static std::uniform_int_distribution<uint64_t> uniformDistribution;

UUID::UUID()
    : uuid(uniformDistribution(engine)) {}

UUID::UUID(uint64_t uuid) : uuid(uuid) {}

UUID::operator uint64_t() const
{
    return uuid;
}

}
