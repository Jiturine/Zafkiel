#pragma once

#include "entity.h"
#include "core/meta/reflection/refl.h"

namespace Zafkiel
{

class WorldSerializer;

class [[refl]] World
{
  public:
    template <typename... Components>
    Entity SpawnEntity(Components &&...components)
    {
        auto handle = registry.create();
        Entity entity(handle, registry);
        UUIDToEntityID[entity.GetUUID()] = handle;
        EntityIDToUUID[handle] = entity.GetUUID();
        if constexpr (sizeof...(Components) != 0)
            SpawnEntityRecursive(handle, std::forward<Components>(components)...);
        return entity;
    }
    Entity SpawnEntityWithUUID(UUID uuid)
    {
        auto handle = registry.create();
        Entity entity(handle, registry, uuid);
        UUIDToEntityID[entity.GetUUID()] = handle;
        EntityIDToUUID[handle] = entity.GetUUID();
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
    Entity GetEntityByID(EntityID id) const
    {
        return Entity(id, registry, EntityIDToUUID.at(id));
    }
    Entity GetEntityByUUID(UUID uuid) const
    {
        return Entity(UUIDToEntityID.at(uuid), registry, uuid);
    }
    std::vector<Entity> AllEntities() const
    {
        std::vector<Entity> entities;
        for (auto entity : registry.view<entt::entity>())
        {
            entities.emplace_back(entity, registry, EntityIDToUUID.at(entity));
        }
        return entities;
    }
    void DestroyEntity(Entity entity)
    {
        registry.destroy(entity.GetHandle());
    }

  private:
    template <typename T, typename... Remains>
    void SpawnEntityRecursive(EntityID handle, T &&component, Remains &&...remains)
    {
        registry.emplace<T>(handle, std::forward<T>(component));
        if constexpr (sizeof...(Remains) != 0)
            SpawnEntityRecursive(handle, std::forward<Remains>(remains)...);
    }
    mutable entt::registry registry;
    std::unordered_map<UUID, EntityID> UUIDToEntityID;
    std::unordered_map<EntityID, UUID> EntityIDToUUID;
};

template <>
struct Serialization<World>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const Any &instance, const Type *typeInfo, YAML::Emitter &out);
    static void Deserialize(Any &instance, const Type *typeInfo, const YAML::Node &data);
};

}