#pragma once

#include "register_base_type.h"
#include "register_property.h"

namespace Zafkiel::Reflection
{
template <typename T>
std::vector<std::pair<std::any, std::shared_ptr<Property>>> GetProperties(T &obj);

std::vector<std::pair<std::any, std::shared_ptr<Property>>> GetProperties(const Class *type, std::any obj);

template <typename T>
T &RemoveRef(std::any obj);
}

#include "register.tpp"