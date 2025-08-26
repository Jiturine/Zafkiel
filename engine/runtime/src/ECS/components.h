#pragma once
#include "base/maths.h"

namespace Zafkiel
{

struct [[refl]] TransformComponent
{
    vec3 position;
    vec3 scale;
};

struct ScriptComponent
{
    std::vector<std::string> scripts;
};

}