#pragma once
#include "editor/resource/asset_loaders/asset_loader.h"

namespace Zafkiel 
{

class MeshLoader final : public AssetLoader
{
  public:
    virtual Ref<Asset> Load(const Ref<EditorAssetMetadata> &metadata) override;
};

}