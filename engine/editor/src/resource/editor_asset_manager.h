#pragma once

#include "asset_metadata.h"
#include "resource/asset_importer.h"
#include "resource/asset_manager.h"
#include "platform/filesystem/filesystem.h"

namespace Zafkiel
{

class EditorAssetManager : public AssetManager
{
  public:
    EditorAssetManager(Ref<GraphicsContext> context);
    virtual Ref<Asset> GetAsset(AssetHandle handle) const override;
    virtual bool IsAssetValid(AssetHandle handle) const override;
    virtual bool IsAssetLoaded(AssetHandle handle) const override;
  private:
    std::unordered_map<AssetHandle, Ref<Asset>> loadedAssets;
    std::unordered_map<AssetHandle, AssetMetadata> assetRegistry;
    Ref<AssetImporter> assetImporter;
};

}