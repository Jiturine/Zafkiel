#pragma once
#include "Core/Meta/Reflection/Refl.h"
#include "Resource/Asset.h"

namespace Zafkiel::Reflection 
{
inline void Register_Resource_Asset() 
{
    ::Zafkiel::Reflection::Register<::Zafkiel::AssetType>("AssetType")
    .Add(::Zafkiel::AssetType::None, "None")
    .Add(::Zafkiel::AssetType::Texture2D, "Texture2D")
    .Add(::Zafkiel::AssetType::CubeMap, "CubeMap")
    .Add(::Zafkiel::AssetType::Model, "Model")
    .Add(::Zafkiel::AssetType::Mesh, "Mesh")
    .Add(::Zafkiel::AssetType::Material, "Material")
    .Add(::Zafkiel::AssetType::Shader, "Shader")
    .Add(::Zafkiel::AssetType::Scene, "Scene")
    .Add(::Zafkiel::AssetType::Font, "Font")
    ;
 

    
}
}