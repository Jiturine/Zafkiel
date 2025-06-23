#pragma once

#include "sparse_set.h"
#include <array>

template <typename EntityID, typename Component, size_t PageSize>
    requires std::is_integral_v<EntityID>
Component &SparseSet<EntityID, Component, PageSize>::Add(EntityID id, Component &&component)
{
    entities.push_back(id);
    components.emplace_back(std::forward<Component>(component));
    Index(id) = entities.size() - 1;
    return components.back();
}

template <typename EntityID, typename Component, size_t PageSize>
    requires std::is_integral_v<EntityID>
void SparseSet<EntityID, Component, PageSize>::Remove(EntityID id)
{
    if (!Contain(id)) return;

    auto &idx = Index(id);
    if (idx == entities.size() - 1)
    {
        idx = null;
        entities.pop_back();
        components.pop_back();
    }
    else
    {
        auto last = entities.back();
        Index(last) = idx;
        std::swap(entities[idx], entities.back());
        components[idx] = std::move(components.back());
        idx = null;
        entities.pop_back();
        components.pop_back();
    }
}
template <typename EntityID, typename Component, size_t PageSize>
    requires std::is_integral_v<EntityID>
bool SparseSet<EntityID, Component, PageSize>::Contain(EntityID id) const
{
    assert(id != null);
    size_t pageIndex = id / PageSize;
    size_t offset = id % PageSize;
    if (pageIndex >= sparsePages.size() || !sparsePages[pageIndex])
        return false;
    return sparsePages[pageIndex]->at(offset) != null;
}

template <typename EntityID, typename Component, size_t PageSize>
    requires std::is_integral_v<EntityID>
void SparseSet<EntityID, Component, PageSize>::Clear()
{
    entities.clear();
    components.clear();
    sparsePages.clear();
}

template <typename EntityID, typename Component, size_t PageSize>
    requires std::is_integral_v<EntityID>
void SparseSet<EntityID, Component, PageSize>::AllocatePage(size_t pageIndex)
{
    if (pageIndex >= sparsePages.size())
    {
        sparsePages.resize(pageIndex + 1);
    }
    sparsePages[pageIndex] = std::make_unique<std::array<EntityID, PageSize>>();
    auto &page = sparsePages[pageIndex];
    page->fill(null);
}

template <typename EntityID, typename Component, size_t PageSize>
    requires std::is_integral_v<EntityID>
EntityID &SparseSet<EntityID, Component, PageSize>::Index(EntityID id)
{
    size_t pageIndex = id / PageSize;
    size_t offset = id % PageSize;
    if (pageIndex >= sparsePages.size() || !sparsePages[pageIndex])
    {
        AllocatePage(pageIndex);
    }
    return sparsePages[pageIndex]->at(offset);
}

template <typename EntityID, typename Component, size_t PageSize>
    requires std::is_integral_v<EntityID>
Component &SparseSet<EntityID, Component, PageSize>::Get(EntityID id)
{
    return components[Index(id)];
}