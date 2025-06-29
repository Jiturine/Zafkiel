#pragma once

#include "entity.h"

namespace Zafkiel
{
class World
{
  public:
    template <typename... Components>
    Entity SpawnEntity(Components &&...components)
    {
        auto handle = registry.create();
        Entity entity(handle, registry);
        if constexpr (sizeof...(Components) != 0)
            SpawnEntityRecursive(handle, std::forward<Components>(components)...);
        return entity;
    }
    template <typename... Components>
    std::vector<Entity> Query()
    {
        std::vector<Entity> entities;
        for (auto entity : registry.view<Components...>())
        {
            entities.push_back(Entity(entity, registry));
        }
        return entities;
    }
    Entity GetEntityByID(EntityID id)
    {
        return Entity(id, registry);
    }
  private:
    template <typename T, typename... Remains>
    void SpawnEntityRecursive(EntityID handle, T &&component, Remains &&...remains)
    {
        registry.emplace<T>(handle, std::forward<T>(component));
        if constexpr (sizeof...(Remains) != 0)
            SpawnEntityRecursive(handle, std::forward<Remains>(remains)...);
    }
    entt::registry registry;
};
}