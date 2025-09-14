#pragma once
#include "core/meta/reflection/refl.h"
#include "resource/asset.h"

namespace Zafkiel::Reflection 
{
inline void Register_resource_asset() 
{
    ::Zafkiel::Reflection::Register<::Zafkiel::AssetType>("AssetType")
    .Add(::Zafkiel::AssetType::None, "None")
    .Add(::Zafkiel::AssetType::Texture2D, "Texture2D")
    ;
 

    
}
}