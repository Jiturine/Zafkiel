#pragma once
#include "reflection/refl.h"

namespace Zafkiel
{
struct [[reflect]] vec3
{
    float x, y, z;
    vec3(float x, float y, float z) : x(x), y(y), z(z) {}
};

}