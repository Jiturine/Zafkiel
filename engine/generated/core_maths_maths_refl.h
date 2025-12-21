#pragma once
#include "core/meta/reflection/refl.h"
#include "core/maths/maths.h"

namespace Zafkiel::Reflection 
{
inline void Register_core_maths_maths() 
{
    

    ::Zafkiel::Reflection::Register<::Zafkiel::vec2>("vec2")
    
    ;
    ::Zafkiel::Reflection::Register<::Zafkiel::vec3>("vec3")
    
    ;
    ::Zafkiel::Reflection::Register<::Zafkiel::vec4>("vec4")
    
    ;
    ::Zafkiel::Reflection::Register<::Zafkiel::uvec2>("uvec2")
    
    ;
    ::Zafkiel::Reflection::Register<::Zafkiel::uvec3>("uvec3")
    
    ;
    ::Zafkiel::Reflection::Register<::Zafkiel::uvec4>("uvec4")
    
    ;
    ::Zafkiel::Reflection::Register<::Zafkiel::ivec2>("ivec2")
    
    ;
    ::Zafkiel::Reflection::Register<::Zafkiel::ivec3>("ivec3")
    
    ;
    ::Zafkiel::Reflection::Register<::Zafkiel::ivec4>("ivec4")
    
    ;
    ::Zafkiel::Reflection::Register<::Zafkiel::quat>("quat")
    
    ;
    ::Zafkiel::Reflection::Register<::Zafkiel::mat3>("mat3")
    
    ;
    ::Zafkiel::Reflection::Register<::Zafkiel::mat4>("mat4")
    
    ;
}
}