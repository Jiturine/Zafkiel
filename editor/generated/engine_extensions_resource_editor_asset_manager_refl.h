#pragma once
#include "core/meta/reflection/refl.h"
#include "engine_extensions/resource/editor_asset_manager.h"

namespace Zafkiel::Reflection 
{
inline void Register_engine_extensions_resource_editor_asset_manager() 
{
    

    ::Zafkiel::Reflection::Register<::Zafkiel::AssetRegistry>("AssetRegistry")
    
    ;
}
}