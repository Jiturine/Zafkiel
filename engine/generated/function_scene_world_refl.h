#pragma once
#include "core/meta/reflection/refl.h"
#include "function/scene/world.h"

namespace Zafkiel::Reflection 
{
inline void Register_function_scene_world() 
{
    

    ::Zafkiel::Reflection::Register<::Zafkiel::World>("World")
    
    ;
}
}