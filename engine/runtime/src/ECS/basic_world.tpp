#pragma once

#include "basic_world.h"

template <typename T>
T &BasicWorld::SetResource(T &&resource)
{
    auto index = IndexGetter<Resource>::Get<T>();
    if (auto it = resources.find(index); it != resources.end())
    {
        auto &resourceInfo = it->second;
        *static_cast<T *>(resources[index].resource) = std::forward<T>(resource);
    }
    else
    {
        auto defaultCreate = []() -> void * { return new T; };
        auto defaultDestroy = [](void *elem) { delete static_cast<T *>(elem); };
        resources.try_emplace(index, defaultCreate, defaultDestroy);
        resources[index].resource = resources[index].create();
        *static_cast<T *>(resources[index].resource) = std::forward<T>(resource);
    }
    return *static_cast<T *>(resources[index].resource);
}

template <typename T>
void BasicWorld::RemoveResource()
{
    auto index = IndexGetter<Resource>::Get<T>();
    if (auto it = resources.find(index); it != resources.end())
    {
        delete static_cast<T *>(it->second.resource);
        it->second.resource = nullptr;
    }
}

template <typename T, typename... Remains>
void BasicWorld::SpawnEntityRecursive(EntityID entity, T &&component, Remains &&...remains)
{
    AddComponent(entity, std::forward<T>(component));
    if constexpr (sizeof...(remains) != 0)
    {
        SpawnEntityRecursive(entity, std::forward<Remains>(remains)...);
    }
}

template <typename... Components>
std::vector<EntityID> BasicWorld::Query()
{
    return QueryRecursive<Components...>();
}

template <typename T, typename... Remains>
bool BasicWorld::HasComponentRecursive(EntityID entity)
{
    bool hasCurrentComponent = false;
    auto cid = IndexGetter<Component>::Get<T>();
    if (auto it = componentMap.find(cid); it != componentMap.end())
    {
        auto componentData = static_cast<ComponentData<T> *>(it->second.get());
        hasCurrentComponent = componentData->sparseSet.Contain(entity);
    }
    if (!hasCurrentComponent) return false;
    if constexpr (sizeof...(Remains) != 0)
    {
        return HasComponentRecursive<Remains...>(entity);
    }
    else
    {
        return true;
    }
}

template <typename T, typename... Remains>
std::vector<EntityID> BasicWorld::QueryRecursive()
{
    std::vector<EntityID> outEntities;
    auto cid = IndexGetter<Component>::Get<T>();
    auto it = componentMap.find(cid);
    if (it == componentMap.end()) return outEntities;
    auto componentData = static_cast<ComponentData<T> *>(it->second.get());
    for (auto entity : componentData->sparseSet.entities)
    {
        if constexpr (sizeof...(Remains) == 0)
        {
            outEntities.push_back(entity);
        }
        else
        {
            if (HasComponent<Remains...>(entity))
                outEntities.push_back(entity);
        }
    }
    return outEntities;
}

template <typename T>
bool BasicWorld::HasResource() const
{
    auto index = IndexGetter<Resource>::Get<T>();
    auto it = resources.find(index);
    return it != resources.end() && it->second.resource;
}

template <typename T>
T &BasicWorld::GetResource() const
{
    auto index = IndexGetter<Resource>::Get<T>();
    return *static_cast<T *>(resources.at(index).resource);
}

template <typename... Components>
bool BasicWorld::HasComponent(EntityID entity)
{
    return HasComponentRecursive<Components...>(entity);
}

template <typename T>
T &BasicWorld::GetComponent(EntityID entity)
{
    auto cid = IndexGetter<Component>::Get<T>();
    auto componentData = static_cast<ComponentData<T> *>(componentMap[cid].get());
    return componentData->sparseSet.Get(entity);
}

template <typename T>
T &BasicWorld::AddComponent(EntityID entity, T &&component)
{
    auto cid = IndexGetter<Component>::Get<T>();
    entities[entity].push_back(cid);
    auto it = componentMap.find(cid);
    if (it == componentMap.end())
    {
        RegisterComponentType<T>();
    }
    auto componentData = static_cast<ComponentData<T> *>(componentMap[cid].get());
    return componentData->sparseSet.Add(entity, std::forward<T>(component));
}

template <typename T>
void BasicWorld::RemoveComponent(EntityID entity)
{
    auto cid = IndexGetter<Component>::Get<T>();
    auto it = componentMap.find(cid);
    if (it != componentMap.end())
    {
        entities[entity].erase(std::find(entities[entity].begin(), entities[entity].end(), cid));
        auto componentData = static_cast<ComponentData<T> *>(componentMap[cid].get());
        componentData->sparseSet.Remove(entity);
    }
}

template <typename T>
void BasicWorld::RegisterComponentType()
{
    auto cid = IndexGetter<Component>::Get<T>();
    if (componentMap.find(cid) != componentMap.end()) return;
    componentMap[cid] = std::make_unique<ComponentData<T>>();
}

template <typename... Components>
EntityID BasicWorld::SpawnEntity(Components &&...components)
{
    EntityID entity = EntityIDGenerator::Get();
    SpawnEntityRecursive(entity, std::forward<Components>(components)...);
    return entity;
}