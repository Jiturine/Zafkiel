#pragma once
#include "editor/Resource/AssetLoaders/AssetLoader.h"
#include "Resource/ModelAsset.h"

namespace Zafkiel 
{

class ModelLoader final : public AssetLoader
{
  public:
    virtual Ref<Asset> Load(const Ref<EditorAssetMetadata> &metadata) override;

};

}