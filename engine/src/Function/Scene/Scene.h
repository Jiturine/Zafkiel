#pragma once
#include "Core/Meta/Serializer/YamlSerializer.h"
#include "Platform/Filesystem/Filesystem.h"
#include "Function/Scene/World.h"
#include "Resource/Asset.h"

namespace Zafkiel
{
class Scene
{
  public:
    Scene(const std::string &data)
    {
        Deserialize(data);
    }

    World &GetWorld() { return world; }
    const World &GetWorld() const { return world; }

    std::string Serialize() const;
    void Deserialize(const std::string &str);

  private:
    World world;
};
}
