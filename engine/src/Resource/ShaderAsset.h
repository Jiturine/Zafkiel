#pragma once

#include "Resource/Asset.h"

namespace Zafkiel
{
class ShaderAsset : public Asset
{
  public:
    virtual AssetType GetAssetType() const override { return AssetType::Shader; }

    ShaderAsset(AssetHandle handle, const Path &path)
        : Asset(handle)
    {
    }

    ~ShaderAsset()
    {
    }
};
}
