#pragma once
#include "base/maths.h"

namespace Zafkiel
{
class Shader : public RefCounted
{
  public:
    virtual ~Shader() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void Set(const std::string &name, const mat4 &matrix) const = 0;
    virtual void Set(const std::string &name, const vec4 &vector) const = 0;
    virtual void Set(const std::string &name, int value) const = 0;
};
}