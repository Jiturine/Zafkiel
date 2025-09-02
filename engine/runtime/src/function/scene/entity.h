#pragma once
#include <entt/entt.hpp>

namespace Zafkiel
{

using EntityID = entt::entity;

class Entity
{
  public:
    Entity(EntityID handle, entt::registry &registry)
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
        return registry.all_of<Components...>(handle);
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
    EntityID GetHandle() const { return handle; }

  private:
    entt::registry &registry;
    EntityID handle;
};
}