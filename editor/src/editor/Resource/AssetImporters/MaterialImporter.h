#pragma once
#include "editor/Resource/EditorAsset.h"

namespace Zafkiel 
{

class MaterialImporter
{
  public:
    Ref<EditorAssetMetadata> Import(const Path &assetPath)
    {
        AssetHandle materialHandle;
        auto materialMetadata = CreateRef<EditorAssetMetadata>(materialHandle, AssetType::Material, assetPath);
        return materialMetadata;
    }
};

}