#pragma once
#include "core/meta/reflection/refl.h"
#include "function/render/shader_family.h"

namespace Zafkiel::Reflection 
{
inline void Register_function_render_shader_family() 
{
    ::Zafkiel::Reflection::Register<::Zafkiel::ShaderFamily>("ShaderFamily")
    .Add(::Zafkiel::ShaderFamily::Unlit, "Unlit")
    .Add(::Zafkiel::ShaderFamily::BlinnPhong, "BlinnPhong")
    ;
 

    
}
}