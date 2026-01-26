#pragma once
#include "editor/resource/asset_importers/asset_importer.h"

namespace Zafkiel 
{

class MaterialImporter final : public AssetImporter
{
  public:
    virtual Ref<EditorAssetMetadata> Import(const Path &assetPath) override
    {
        AssetHandle materialHandle;
        auto materialMetadata = CreateRef<EditorAssetMetadata>(materialHandle, AssetType::Material, assetPath);
        return materialMetadata;
    }
};

}