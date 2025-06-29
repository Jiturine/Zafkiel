#pragma once
#include "base/vector3.h"

namespace Zafkiel
{

struct [[reflect]] TransformComponent
{
    vec3 position;
    vec3 scale;
};

struct ScriptComponent
{
    std::vector<std::string> scripts;
};

}