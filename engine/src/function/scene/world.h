#pragma once

#include "entity.h"
#include "core/meta/reflection/refl.h"
#include "resource/asset.h"

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
    std::vector<Entity> Query() const
    {
        std::vector<Entity> entities;
        for (auto entity : registry.view<Components...>())
        {
            entities.push_back(Entity(entity, registry, EntityIDToUUID.at(entity)));
        }
        return entities;
    }
    bool HasEntity(EntityID id) const
    {
        return registry.valid(id);
    }
    bool HasEntity(UUID uuid) const
    {
        return UUIDToEntityID.contains(uuid);
    }
    bool HasEntity(Entity entity) const
    {
        return registry.valid(entity.GetHandle()) && &registry == entity.GetRegistry();
    }

    Entity GetEntityByID(EntityID id) const
    {
        return Entity(id, registry, EntityIDToUUID.at(id));
    }
    Entity GetEntityByUUID(UUID uuid) const
    {
        auto it = UUIDToEntityID.find(uuid);
        if (it != UUIDToEntityID.end())
        {
            return Entity(it->second, registry, uuid);
        }
        Log::CoreError("Cannot Find Entity: {}", (uint64_t)uuid);
        return Entity();
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
    std::vector<UUID> AllEntityUUIDs() const
    {
        std::vector<UUID> uuids;
        for (auto &[uuid, _] : UUIDToEntityID)
        {
            uuids.push_back(uuid);
        }
        return uuids;
    }
    void DestroyEntity(Entity entity)
    {
        EntityIDToUUID.erase(entity.GetHandle());
        UUIDToEntityID.erase(entity.GetUUID());
        registry.destroy(entity.GetHandle());
    }
    Entity InstantiateModel(AssetHandle model);

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
    static void Serialize(const Any instance, Any context, YAML::Emitter &out);
    static void Deserialize(Any instance, Any context, const YAML::Node &data);
};

}