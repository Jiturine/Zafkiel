#pragma once

#include "Resource/Asset.h"

namespace Zafkiel
{

class AssetManager
{
  public:
    virtual ~AssetManager() = default;

    virtual Ref<Asset> GetAsset(AssetHandle handle) = 0;
    virtual Ref<Asset> LoadAsset(AssetHandle handle) = 0;
    virtual Ref<AssetMetadata> GetAssetMetadata(AssetHandle handle) = 0;
    virtual bool IsAssetValid(AssetHandle handle) const = 0;
    virtual bool IsAssetLoaded(AssetHandle handle) const = 0;

    static AssetManager &Instance() { return *instancePtr; }

  protected:
    inline static AssetManager *instancePtr;

};
}