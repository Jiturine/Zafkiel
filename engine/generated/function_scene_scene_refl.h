#pragma once
#include "core/meta/reflection/refl.h"
#include "function/scene/scene.h"

namespace Zafkiel::Reflection 
{
inline void Register_function_scene_scene() 
{
    

    ::Zafkiel::Reflection::Register<::Zafkiel::World>("World")
    
    ;
}
}