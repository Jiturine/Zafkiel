#pragma once

#include <entt/entt.hpp>
#include <utility>
#include "entity.h"
#include "entt/entity/fwd.hpp"

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
    auto Query() const
    {
        return registry.view<Components...>();
    }
  private:
    template <typename T, typename... Remains>
    void SpawnEntityRecursive(entt::entity handle, T &&component, Remains &&...remains)
    {
        registry.emplace<T>(handle, std::forward<T>(component));
        if constexpr (sizeof...(Remains) != 0)
            SpawnEntityRecursive(handle, remains...);
    }
    entt::registry registry;
};