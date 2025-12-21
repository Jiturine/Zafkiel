#pragma once
#include "editor/resource/asset_loaders/asset_loader.h"
#include "resource/material_asset.h"

namespace Zafkiel 
{

class MaterialLoader final : public AssetLoader
{
  public:
    virtual Ref<Asset> Load(const Ref<EditorAssetMetadata> &metadata) override;
};

}