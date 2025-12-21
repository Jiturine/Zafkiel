#pragma once
#include "asset_importer.h"

namespace Zafkiel 
{

class SceneImporter final : public AssetImporter
{
  public:
    virtual Ref<EditorAssetMetadata> Import(const Path &assetPath) override
    {
        AssetHandle handle;
        auto metadata = CreateRef<EditorAssetMetadata>(handle, AssetType::Scene, assetPath);
        return metadata; 
    }
};

}