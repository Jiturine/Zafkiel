#pragma once
#include "core/meta/serializer/yaml_serializer.h"
#include "platform/filesystem/filesystem.h"
#include "function/scene/world.h"
#include "resource/asset.h"

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
