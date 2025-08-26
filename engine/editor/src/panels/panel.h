#pragma once
#include "base/maths.h"

namespace Zafkiel
{

class Panel
{
  public:
    virtual ~Panel() = default;
    virtual void Render() = 0;
    vec2 size = vec2(1.0f, 1.0f);
};
}