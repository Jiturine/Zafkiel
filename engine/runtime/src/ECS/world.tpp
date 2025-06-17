#pragma once

#include "world.h"

template <typename... Components>
Entity World::SpawnEntity(Components &&...components)
{
    return Entity(basicWorld.SpawnEntity<Components...>(std::forward<Components>(components)...), basicWorld);
}

void World::Destroy(EntityID entity)
{
    basicWorld.Destroy(entity);
}

template <typename T>
T &World::SetResource(T &&resource)
{
    return basicWorld.SetResource(std::forward<T>(resource));
}

template <typename T>
void World::RemoveResource()
{
    basicWorld.RemoveResource<T>();
}

template <typename T>
bool World::HasResource() const
{
    return basicWorld.HasResource<T>();
}

template <typename T>
T &World::GetResource() const
{
    return basicWorld.GetResource<T>();
}

template <typename... Components>
std::vector<Entity> World::Query()
{
    std::vector<Entity> outEntities;
    for (auto entity : basicWorld.Query<Components...>())
    {
        outEntities.push_back(Entity(entity, basicWorld));
    }
    return outEntities;
}