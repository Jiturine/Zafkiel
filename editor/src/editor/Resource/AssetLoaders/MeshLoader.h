#pragma once
#include "editor/Resource/AssetLoaders/AssetLoader.h"

namespace Zafkiel 
{

class MeshLoader final : public AssetLoader
{
  public:
    virtual Ref<Asset> Load(const Ref<EditorAssetMetadata> &metadata) override;
};

}