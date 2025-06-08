#pragma once
#include "reflection/refl.h"

namespace Zafkiel
{
struct [[reflect]] vec3
{
    float x, y, z;
};

struct [[reflect]] Camera
{
    vec3 position;
    vec3 lookAt;
    float fov;
};
}