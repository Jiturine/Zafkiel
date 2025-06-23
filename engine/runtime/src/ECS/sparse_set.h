#pragma once

#include <limits>

template <typename EntityID, typename Component, size_t PageSize>
    requires std::is_integral_v<EntityID>
class SparseSet final
{
  public:
    Component &Add(EntityID id, Component &&component);

    Component &Get(EntityID id);

    void Remove(EntityID id);

    bool Contain(EntityID id) const;

    void Clear();

    auto begin() { return entities.begin(); }

    auto end() { return entities.end(); }

    std::vector<EntityID> entities;
    std::vector<Component> components;
    std::vector<std::unique_ptr<std::array<EntityID, PageSize>>> sparsePages;
  private:
    static constexpr EntityID null = std::numeric_limits<EntityID>::max();
    size_t Page(EntityID t) const { return t / PageSize; }

    size_t Offset(EntityID t) const { return t % PageSize; }

    EntityID &Index(EntityID id);

    void AllocatePage(size_t pageIndex);
};

#include "sparse_set.tpp"