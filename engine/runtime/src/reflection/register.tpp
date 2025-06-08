#pragma once
#include "register.h"

namespace Zafkiel::Reflection
{
template <typename T>
std::vector<std::pair<std::any, std::shared_ptr<Property>>> GetProperties(T &obj)
{
    const Class *type = GetType<T>()->template As<Class>();
    std::any a = std::ref(obj);
    std::vector<std::pair<std::any, std::shared_ptr<Property>>> ret;
    for (const auto &prop : type->GetProperties())
    {
        std::any val_ptr = prop->Call(a);
        ret.emplace_back(val_ptr, prop);
    }
    return ret;
}

std::vector<std::pair<std::any, std::shared_ptr<Property>>> GetProperties(const Class *type, std::any obj)
{
    std::vector<std::pair<std::any, std::shared_ptr<Property>>> ret;
    for (const auto &prop : type->GetProperties())
    {
        std::any val_ptr = prop->Call(obj);
        ret.emplace_back(val_ptr, prop);
    }
    return ret;
}

template <typename T>
T &RemoveRef(std::any obj)
{
    return std::any_cast<std::reference_wrapper<T>>(obj).get();
}
}