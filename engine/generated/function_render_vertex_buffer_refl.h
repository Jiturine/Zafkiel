#pragma once
#include "core/meta/reflection/refl.h"
#include "function/render/vertex_buffer.h"

namespace Zafkiel::Reflection 
{
inline void Register_function_render_vertex_buffer() 
{
    ::Zafkiel::Reflection::Register<::Zafkiel::ShaderReflection::FundamentalKind>("FundamentalKind")
    .Add(::Zafkiel::ShaderReflection::FundamentalKind::None, "None")
    .Add(::Zafkiel::ShaderReflection::FundamentalKind::Float, "Float")
    .Add(::Zafkiel::ShaderReflection::FundamentalKind::Float2, "Float2")
    .Add(::Zafkiel::ShaderReflection::FundamentalKind::Float3, "Float3")
    .Add(::Zafkiel::ShaderReflection::FundamentalKind::Float4, "Float4")
    .Add(::Zafkiel::ShaderReflection::FundamentalKind::Mat3, "Mat3")
    .Add(::Zafkiel::ShaderReflection::FundamentalKind::Mat4, "Mat4")
    .Add(::Zafkiel::ShaderReflection::FundamentalKind::Int, "Int")
    .Add(::Zafkiel::ShaderReflection::FundamentalKind::Int2, "Int2")
    .Add(::Zafkiel::ShaderReflection::FundamentalKind::Int3, "Int3")
    .Add(::Zafkiel::ShaderReflection::FundamentalKind::Int4, "Int4")
    .Add(::Zafkiel::ShaderReflection::FundamentalKind::UInt, "UInt")
    .Add(::Zafkiel::ShaderReflection::FundamentalKind::UInt2, "UInt2")
    .Add(::Zafkiel::ShaderReflection::FundamentalKind::UInt3, "UInt3")
    .Add(::Zafkiel::ShaderReflection::FundamentalKind::UInt4, "UInt4")
    .Add(::Zafkiel::ShaderReflection::FundamentalKind::Bool, "Bool")
    ;
 

    
}
}