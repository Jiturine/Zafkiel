#pragma once

#include "asset.h"

namespace Zafkiel
{

class AssetManager : public RefCounted
{
  public:
    virtual ~AssetManager() = default;

    virtual Ref<Asset> GetAsset(AssetHandle handle) = 0;
    virtual bool IsAssetValid(AssetHandle handle) const = 0;
    virtual bool IsAssetLoaded(AssetHandle handle) const = 0;
};
}