#pragma once
#include "core/meta/reflection/refl.h"
#include "resource/asset_manager.h"

namespace Zafkiel::Reflection 
{
inline void Register_resource_asset_manager() 
{
    ::Zafkiel::Reflection::Register<::Zafkiel::AssetType>("AssetType")
    .Add(::Zafkiel::AssetType::None, "None")
    .Add(::Zafkiel::AssetType::Texture2D, "Texture2D")
    .Add(::Zafkiel::AssetType::Model, "Model")
    .Add(::Zafkiel::AssetType::Mesh, "Mesh")
    ;
 

    
}
}