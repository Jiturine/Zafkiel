#pragma once

#include "ECS/sparse_set.h"
#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>
#include <new>
#include <strings.h>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
using ComponentID = uint32_t;
using EntityID = uint32_t;

enum Category
{
    Resource,
    Component
};

template <Category category>
class IndexGetter final
{
  public:
    template <typename T>
    static uint32_t Get()
    {
        static uint32_t id = curIdx++;
        return id;
    }
  private:
    inline static uint32_t curIdx = 0;
};

template <typename T>
    requires std::is_integral_v<T>
class IDGenerator final
{
  public:
    static T Get()
    {
        return curIdx++;
    }
  private:
    inline static T curIdx = 0;
};
using EntityIDGenerator = IDGenerator<EntityID>;

class BasicWorld final
{
  public:
    BasicWorld() = default;
    BasicWorld(const BasicWorld &) = delete;
    BasicWorld &operator=(const BasicWorld &) = delete;

    template <typename... Components>
    EntityID SpawnEntity(Components &&...components);

    void Destroy(EntityID entity);

    template <typename T>
    T &SetResource(T &&resource);

    template <typename T>
    void RemoveResource();

    template <typename T>
    bool HasResource() const;

    template <typename T>
    T &GetResource() const;

    template <typename T>
    void RegisterComponentType();

    template <typename... Components>
    bool HasComponent(EntityID entity);

    template <typename T>
    T &GetComponent(EntityID entity);

    template <typename T>
    T &AddComponent(EntityID entity, T &&component);

    template <typename T>
    void RemoveComponent(EntityID entity);

    template <typename... Components>
    std::vector<EntityID> Query();

  private:
    template <typename T, typename... Remains>
    bool HasComponentRecursive(EntityID entity);

    template <typename T, typename... Remains>
    std::vector<EntityID> QueryRecursive();

    struct IComponentData
    {
        virtual ~IComponentData() = default;
        virtual void Remove(EntityID entity) = 0;
    };

    template <typename T>
    struct ComponentData final : IComponentData // final 去虚拟化
    {
        virtual void Remove(EntityID entity) final override { sparseSet.Remove(entity); }
        SparseSet<EntityID, T, 4096> sparseSet;
    };

    template <typename T, typename... Remains>
    void SpawnEntityRecursive(EntityID entity, T &&component, Remains &&...remains);

    struct ResourceInfo
    {
        void *resource;
        std::function<void *(void)> create;
        std::function<void(void *)> destroy;

        ResourceInfo() = default;
        ResourceInfo(std::function<void *(void)> create, const std::function<void(void *)> destroy)
            : create(create), destroy(destroy)
        {
            if (!create) assert(false);
            if (!destroy) assert(false);
        }
        ~ResourceInfo()
        {
            destroy(resource);
        }
    };
    std::unordered_map<ComponentID, std::unique_ptr<IComponentData>> componentMap;
    std::unordered_map<EntityID, std::vector<ComponentID>> entities;
    std::unordered_map<ComponentID, ResourceInfo> resources;
};

#include "basic_world.tpp"