#pragma once
#include <string>
#include "reflection/refl.h"

namespace Zafkiel
{
struct [[reflect]] vec3
{
    float x, y, z;
    int id;
};
}