#pragma once

#include "resource/asset_manager.h"
#include "platform/filesystem/filesystem.h"
#include "function/render/graphics_context.h"

namespace Zafkiel
{
struct [[refl]] AssetMetadata
{
    AssetType type;
    Path filePath;
};

class EditorAssetManager : public AssetManager
{
  public:
    EditorAssetManager();
    void SetAssetDirectory(const Path &path);
    Path GetAssetDirectory();
    AssetHandle ImportAsset(const Path &assetPath);
    void SerializeAssetRegistry();
    void DeserializeAssetRegistry();
    virtual Ref<Asset> GetAsset(AssetHandle handle) override;
    virtual bool IsAssetValid(AssetHandle handle) const override;
    virtual bool IsAssetLoaded(AssetHandle handle) const override;

  private:
    Ref<Asset> LoadAsset(const AssetMetadata &metadata) const;
    Ref<Asset> LoadTexture2D(const Path &path) const;
    std::unordered_map<AssetHandle, Ref<Asset>> loadedAssets;
    std::unordered_map<AssetHandle, AssetMetadata> assetRegistry;
    Path assetDirectory;
};

}