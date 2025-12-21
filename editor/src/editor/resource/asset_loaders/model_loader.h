#pragma once
#include "editor/resource/asset_loaders/asset_loader.h"
#include "resource/model_asset.h"

namespace Zafkiel 
{

class ModelLoader final : public AssetLoader
{
  public:
    virtual Ref<Asset> Load(const Ref<EditorAssetMetadata> &metadata) override;

};

}