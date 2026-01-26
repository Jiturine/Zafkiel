#pragma once

#include "resource/asset.h"

namespace Zafkiel
{

class AssetManager
{
  public:
    virtual ~AssetManager() = default;

    static Ref<Asset> GetAsset(AssetHandle handle) { return instancePtr->GetAssetImpl(handle); }
    static Ref<Asset> LoadAsset(AssetHandle handle) { return instancePtr->LoadAssetImpl(handle); }
    static Ref<AssetMetadata> GetAssetMetadata(AssetHandle handle) { return instancePtr->GetAssetMetadataImpl(handle); }
    static bool IsAssetValid(AssetHandle handle) { return instancePtr->IsAssetValidImpl(handle); }
    static bool IsAssetLoaded(AssetHandle handle) { return instancePtr->IsAssetLoadedImpl(handle); }
  
    static AssetManager &Instance() { return *instancePtr; }

  protected:
    inline static AssetManager *instancePtr;

    virtual Ref<Asset> GetAssetImpl(AssetHandle handle) = 0;
    virtual Ref<Asset> LoadAssetImpl(AssetHandle handle) = 0;
    virtual Ref<AssetMetadata> GetAssetMetadataImpl(AssetHandle handle) = 0;
    virtual bool IsAssetValidImpl(AssetHandle handle) const = 0;
    virtual bool IsAssetLoadedImpl(AssetHandle handle) const = 0;
};
}