#pragma once

namespace Zafkiel::Reflection
{

template <typename>
struct property_traits;

template <typename Class_, typename Value_>
struct property_traits<Value_ Class_::*>
{
    using ClassType = Class_;
    using ValueType = Value_;
};

}
