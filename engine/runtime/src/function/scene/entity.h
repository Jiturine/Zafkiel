#pragma once
#include <entt/entt.hpp>

namespace Zafkiel
{

using EntityID = entt::entity;

class Entity
{
  public:
    Entity() : registry(nullptr), handle(entt::null) {}

    Entity(EntityID handle, entt::registry &registry)
        : registry(&registry), handle(handle) {}

    Entity &operator=(const Entity &other)
    {
        registry = other.registry;
        handle = other.handle;
        return *this;
    }

    Entity(const Entity &other) = default;

    Entity(Entity &&other) noexcept
        : registry(other.registry), handle(other.handle)
    {
        other.registry = nullptr;
        other.handle = entt::null;
    }

    Entity &operator=(Entity &&other) noexcept
    {
        if (this != &other)
        {
            registry = other.registry;
            handle = other.handle;
            other.registry = nullptr;
            other.handle = entt::null;
        }
        return *this;
    }

    bool operator==(const Entity &other) const
    {
        return handle == other.handle && registry == other.registry;
    }

    operator bool() const
    {
        return handle != entt::null && registry;
    }

    template <typename T>
    T &AddComponent(T &&component)
    {
        return registry->emplace<T>(handle, std::forward<T>(component));
    }

    template <typename T>
    T &GetComponent() const
    {
        return registry->get<T>(handle);
    }

    template <typename... Components>
    bool HasComponent() const
    {
        return registry->all_of<Components...>(handle);
    }

    template <typename T>
    void RemoveComponent()
    {
        registry->erase<T>(handle);
    }

    void Destroy()
    {
        registry->destroy(handle);
    }
    EntityID GetHandle() const { return handle; }

  private:
    entt::registry *registry;
    EntityID handle;
};
}