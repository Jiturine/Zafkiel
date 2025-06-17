#pragma once

#include <limits>

template <typename T, size_t PageSize>
    requires std::is_integral_v<T>
class SparseSet final
{
  public:
    void Add(T t);

    void Remove(T t);

    bool Contain(T t) const;

    void Clear();

    auto begin() { return density.begin(); }

    auto end() { return density.end(); }

  private:
    std::vector<T> density;
    std::vector<std::unique_ptr<std::array<T, PageSize>>> sparse;
    static constexpr T null = std::numeric_limits<T>::max();
    size_t Page(T t) const { return t / PageSize; }

    size_t Offset(T t) const { return t % PageSize; }

    T &Index(T t) { return sparse[Page(t)]->at(Offset(t)); }

    void Expand(size_t p);
};

#include "sparse_set.tpp"