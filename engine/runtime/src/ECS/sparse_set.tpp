#pragma once

#include "sparse_set.h"

template <typename T, size_t PageSize>
    requires std::is_integral_v<T>
void SparseSet<T, PageSize>::Add(T t)
{
    density.push_back(t);
    if (Page(t) >= sparse.size())
    {
        Expand(Page(t));
    }
    Index(t) = density.size() - 1;
}

template <typename T, size_t PageSize>
    requires std::is_integral_v<T>
void SparseSet<T, PageSize>::Remove(T t)
{
    if (!Contain(t)) return;

    auto &idx = Index(t);
    if (idx == density.size() - 1)
    {
        idx = null;
        density.pop_back();
    }
    else
    {
        auto last = density.back();
        Index(last) = idx;
        std::swap(density[idx], density.back());
        idx = null;
        density.pop_back();
    }
}
template <typename T, size_t PageSize>
    requires std::is_integral_v<T>
bool SparseSet<T, PageSize>::Contain(T t) const
{
    assert(t != null);
    return Page(t) < sparse.size() && sparse[Page(t)]->at(Offset(t)) != null;
}

template <typename T, size_t PageSize>
    requires std::is_integral_v<T>
void SparseSet<T, PageSize>::Clear()
{
    density.clear();
    sparse.clear();
}

template <typename T, size_t PageSize>
    requires std::is_integral_v<T>
void SparseSet<T, PageSize>::Expand(size_t p)
{
    for (size_t i = sparse.size(); i <= p; i++)
    {
        sparse.emplace_back(std::make_unique<std::array<T, PageSize>>());
        sparse[i]->fill(null);
    }
}