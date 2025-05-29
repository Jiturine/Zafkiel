#include "Any.h"

namespace Reflection
{

// 实现swap & copy

Any::Any(const Any &other) : typeinfo(other.typeinfo), storageType(other.storageType), ops(other.ops)
{
    if (ops.copy)
    {
        auto new_any = ops.copy(other);
        payload = new_any.payload;
        new_any.payload = nullptr;
        new_any.ops.release = nullptr;
    }
    else
    {
        storageType = StorageType::Empty;
        typeinfo = nullptr;
    }
}

void swap(Any &a, Any &b)
{
    using std::swap;
    swap(a.ops, b.ops);
    swap(a.payload, b.payload);
    swap(a.storageType, b.storageType);
    swap(a.typeinfo, b.typeinfo);
}

}