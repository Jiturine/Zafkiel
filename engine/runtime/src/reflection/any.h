#pragma once
#include <functional>

namespace Zafkiel::Reflection
{

class Any;
class Type;
void swap(Any &a, Any &b);

// 用了 swap & copy 范式来写了拷贝移动构造和赋值

class Any final
{
  public:
    enum class StorageType
    {
        Empty,
        Copy,
        Move,
        Ref,
        ConstRef
    };
    struct Operations
    {
        std::function<Any(const Any &)> copy;
        std::function<Any(Any &&)> move;
        std::function<void(Any &)> release;
    };
    Any() = default;
    Any(const Type *typeinfo, void *payload, StorageType storageType, Operations ops)
        : typeinfo(typeinfo), payload(payload), storageType(storageType), ops(ops)
    {
    }
    Any(const Any &other);
    Any(Any &&other) noexcept : Any() { swap(other, *this); }
    // 同时处理拷贝和移动赋值
    Any &operator=(Any other)
    {
        swap(other, *this);
        return *this;
    }
    ~Any()
    {
        if (ops.release && (storageType == StorageType::Copy || storageType == StorageType::Move))
        {
            ops.release(*this);
        }
    }
    const Type *typeinfo;
    void *payload;
    StorageType storageType = StorageType::Empty;
    Operations ops;
};

// 定义每种类型构建出的Any类型要如何进行拷贝，移动，释放
template <typename T>
struct operations_traits
{
    static Any Copy(const Any &elem);
    static Any Move(Any &&elem);
    static void Release(Any &elem);
    static const Any::Operations &GetOperations();
};
}

#include "any.tpp"
