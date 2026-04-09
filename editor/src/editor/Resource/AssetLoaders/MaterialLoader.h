#pragma once
#include "editor/Resource/AssetLoaders/AssetLoader.h"
#include "Resource/MaterialAsset.h"

namespace Zafkiel 
{

class MaterialLoader final : public AssetLoader
{
  public:
    virtual Ref<Asset> Load(const Ref<EditorAssetMetadata> &metadata) override;
};

}