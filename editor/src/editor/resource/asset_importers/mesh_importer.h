#pragma once
#include "asset_importer.h"

namespace Zafkiel 
{

class MeshImporter final : public AssetImporter
{
  public:
    virtual Ref<EditorAssetMetadata> Import(const Path &assetPath) override
    {
        AssetHandle meshHandle;
        auto meshMetadata = CreateRef<EditorAssetMetadata>(meshHandle, AssetType::Mesh, assetPath);
        return meshMetadata;
    }
};

}