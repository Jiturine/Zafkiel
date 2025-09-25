#pragma once
#include <entt/entt.hpp>
#include "core/base/uuid.h"

namespace Zafkiel
{

using EntityID = entt::entity;

class Entity
{
  public:
    Entity() : registry(nullptr), handle(entt::null), uuid() {}

    Entity(EntityID handle, entt::registry &registry, UUID uuid = UUID())
        : registry(&registry), handle(handle), uuid(uuid) {}

    Entity &operator=(const Entity &other) = default;
    Entity(const Entity &other) = default;

    Entity(Entity &&other) noexcept
        : registry(other.registry), handle(other.handle), uuid(other.uuid)
    {
        other.registry = nullptr;
        other.handle = entt::null;
        other.uuid = 0;
    }

    Entity &operator=(Entity &&other) noexcept
    {
        if (this != &other)
        {
            registry = other.registry;
            handle = other.handle;
            uuid = other.uuid;
            other.registry = nullptr;
            other.handle = entt::null;
            other.uuid = 0;
        }
        return *this;
    }

    bool operator==(const Entity &other) const
    {
        return handle == other.handle && uuid == other.uuid && registry == other.registry;
    }

    operator bool() const
    {
        return handle != entt::null && uuid != 0 && registry;
    }

    template <typename T>
    T &AddComponent(T &&component)
    {
        return registry->emplace<std::decay_t<T>>(handle, std::forward<T>(component));
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

    bool HasParent() const;

    Entity GetParent() const;

    bool HasChildren() const;

    std::vector<Entity> GetChildren() const;

    UUID GetUUID() const;

    EntityID GetHandle() const { return handle; }

    const entt::registry *GetRegistry() const { return registry; }

  private:
    entt::registry *registry;
    EntityID handle;
    UUID uuid;
};
}