#pragma once
#include "Core/Meta/Reflection/Refl.h"
#include "Resource/MaterialAsset.h"

namespace Zafkiel::Reflection 
{
inline void Register_Resource_MaterialAsset() 
{
    ::Zafkiel::Reflection::Register<::Zafkiel::ShaderFamily>("ShaderFamily")
    .Add(::Zafkiel::ShaderFamily::Unlit, "Unlit")
    .Add(::Zafkiel::ShaderFamily::BlinnPhong, "BlinnPhong")
    .Add(::Zafkiel::ShaderFamily::PBR, "PBR")
    ;
 
    ::Zafkiel::Reflection::Register<::Zafkiel::MaterialAssetParameterType>("MaterialAssetParameterType")
    .Add(::Zafkiel::MaterialAssetParameterType::None, "None")
    .Add(::Zafkiel::MaterialAssetParameterType::SampledImage, "SampledImage")
    .Add(::Zafkiel::MaterialAssetParameterType::Vec3, "Vec3")
    .Add(::Zafkiel::MaterialAssetParameterType::Vec4, "Vec4")
    .Add(::Zafkiel::MaterialAssetParameterType::Mat3, "Mat3")
    .Add(::Zafkiel::MaterialAssetParameterType::Mat4, "Mat4")
    ;
 

    
}
}