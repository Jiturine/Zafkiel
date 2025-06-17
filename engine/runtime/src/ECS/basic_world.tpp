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
    auto index = IndexGetter<Component>::Get<T>();
    if (auto it = componentMap.find(index); it == componentMap.end())
    {
        auto defaultCreate = []() -> void * { return new T; };
        auto defaultDestroy = [](void *elem) { delete static_cast<T *>(elem); };
        componentMap[index] = ComponentInfo(defaultCreate, defaultDestroy);
    }
    auto &componentInfo = componentMap[index];
    void *elem = componentInfo.pool.Create();
    *static_cast<T *>(elem) = std::forward<T>(component);
    componentInfo.sparseSet.Add(entity);

    entities[entity][index] = elem;

    if constexpr (sizeof...(remains) != 0)
    {
        SpawnEntityRecursive(entity, std::forward<Remains>(remains)...);
    }
}

void *BasicWorld::Pool::Create()
{
    if (!cache.empty())
    {
        instances.push_back(cache.back());
        cache.pop_back();
    }
    else
    {
        instances.push_back(create());
    }
    return instances.back();
}

void BasicWorld::Pool::Destroy(void *elem)
{
    if (auto it = std::find(instances.begin(), instances.end(), elem); it != instances.end())
    {
        cache.push_back(*it);
        std::swap(*it, instances.back());
        instances.pop_back();
    }
    else
    {
        assert(false);
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
    auto it = entities.find(entity);
    auto index = IndexGetter<Component>::Get<T>();
    bool hasCurrentComponent = it != entities.end() && it->second.find(index) != it->second.end();
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
    auto index = IndexGetter<Component>::Get<T>();
    auto &componentInfo = componentMap[index];
    for (auto entity : componentInfo.sparseSet)
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
    auto index = IndexGetter<Component>::Get<T>();
    return *static_cast<T *>(entities[entity][index]);
}

template <typename T>
T &BasicWorld::AddComponent(EntityID entity, T &&component)
{
    auto index = IndexGetter<Component>::Get<T>();
    if (auto it = componentMap.find(index); it == componentMap.end())
    {
        auto defaultCreate = []() -> void * { return new T; };
        auto defaultDestroy = [](void *elem) { delete static_cast<T *>(elem); };
        componentMap[index] = ComponentInfo(defaultCreate, defaultDestroy);
    }
    auto &componentInfo = componentMap[index];
    void *elem = componentInfo.pool.Create();
    *static_cast<T *>(elem) = std::forward<T>(component);
    componentInfo.sparseSet.Add(entity);
    entities[entity][index] = elem;
    return *static_cast<T *>(elem);
}

template <typename... Components>
EntityID BasicWorld::SpawnEntity(Components &&...components)
{
    EntityID entity = EntityIDGenerator::Get();
    SpawnEntityRecursive(entity, std::forward<Components>(components)...);
    return entity;
}