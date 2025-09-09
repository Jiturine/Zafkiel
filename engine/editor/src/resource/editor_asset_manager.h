#pragma once

#include "resource/asset_manager.h"
#include "platform/filesystem/filesystem.h"
#include "function/render/graphics_context.h"

namespace Zafkiel
{
struct AssetMetadata
{
    AssetType type;
    Path filePath;
};

class EditorAssetManager : public AssetManager
{
  public:
    EditorAssetManager(Ref<GraphicsContext> context);
    void SetAssetPath(const Path &path);
    AssetHandle ImportAsset(const Path &assetPath);
    virtual Ref<Asset> GetAsset(AssetHandle handle) override;
    virtual bool IsAssetValid(AssetHandle handle) const override;
    virtual bool IsAssetLoaded(AssetHandle handle) const override;
  private:
    Ref<Asset> LoadAsset(const AssetMetadata &metadata) const;
    Ref<Asset> LoadTexture2D(const Path &path) const;
    std::unordered_map<AssetHandle, Ref<Asset>> loadedAssets;
    std::unordered_map<AssetHandle, AssetMetadata> assetRegistry;
    Ref<GraphicsContext> context;
    Path assetDirectoryPath;
};

}