#pragma once
#include "world.h"

namespace Zafkiel
{
class Scene : public RefCounted
{
  public:
    World &GetWorld()
    {
        return world;
    }
    const World &GetWorld() const { return world; }
    Entity selectedEntity;
  private:
    World world;
};
}