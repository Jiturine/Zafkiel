#pragma once
#include "core/meta/reflection/refl.h"
#include "resource/editor_asset_manager.h"

namespace Zafkiel::Reflection 
{
inline void Register_resource_editor_asset_manager() 
{
    

    ::Zafkiel::Reflection::Register<::Zafkiel::AssetMetadata>("AssetMetadata")
    .AddProperty(&::Zafkiel::AssetMetadata::type, "type")
    .AddProperty(&::Zafkiel::AssetMetadata::filePath, "filePath")
    ;
}
}