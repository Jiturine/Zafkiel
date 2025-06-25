#pragma once
#include "entt/entity/fwd.hpp"
#include <entt/entt.hpp>

class Entity
{
  public:
    Entity(entt::entity handle, entt::registry &registry)
        : registry(registry), handle(handle) {}

    template <typename T>
    T &AddComponent(T &&component)
    {
        return registry.emplace<T>(handle, std::forward<T>(component));
    }

    template <typename T>
    T &GetComponent() const
    {
        return registry.get<T>(handle);
    }

    template <typename... Components>
    bool HasComponent() const
    {
        return registry.all_of<Components...>();
    }

    template <typename T>
    void RemoveComponent()
    {
        registry.erase<T>(handle);
    }

    void Destroy()
    {
        registry.destroy(handle);
    }

  private:
    entt::registry &registry;
    entt::entity handle;
};