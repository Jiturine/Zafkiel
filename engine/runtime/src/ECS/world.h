#pragma once

#include "basic_world.h"
#include "entity.h"

class World
{
  public:
    World() = default;
    World(const World &) = delete;
    World &operator=(const World &) = delete;

    template <typename... Components>
    Entity SpawnEntity(Components &&...components);

    void Destroy(EntityID entity);

    template <typename T>
    T &SetResource(T &&resource);

    template <typename T>
    void RemoveResource();

    template <typename T>
    bool HasResource() const;

    template <typename T>
    T &GetResource() const;

    template <typename... Components>
    std::vector<Entity> Query();

  private:
    BasicWorld basicWorld;
};

#include "world.tpp"