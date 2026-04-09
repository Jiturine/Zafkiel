#pragma once
#include "editor/Resource/EditorAsset.h"

namespace Zafkiel 
{

class SceneImporter
{
  public:
    Ref<EditorAssetMetadata> Import(const Path &assetPath)
    {
        AssetHandle handle;
        auto metadata = CreateRef<EditorAssetMetadata>(handle, AssetType::Scene, assetPath);
        return metadata; 
    }
};

}