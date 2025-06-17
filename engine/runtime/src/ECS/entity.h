#pragma once

#include "basic_world.h"

class Entity
{
  public:
    Entity(EntityID id, BasicWorld &world) : id(id), world(world) {}
    ~Entity() = default;

    template <typename... Components>
    bool HasComponent() const
    {
        return world.HasComponent<Components...>(id);
    }
    template <typename T>
    T &GetComponent() const
    {
        return world.GetComponent<T>(id);
    }
    template <typename T>
    T &AddComponent(T &&component)
    {
        return world.AddComponent<T>(id, std::forward<T>(component));
    }
    operator EntityID() const { return id; }
  private:
    BasicWorld &world;
    EntityID id;
};