#pragma once
#include "editor/Resource/AssetLoaders/AssetLoader.h"
#include "editor/Resource/EditorAsset.h"

namespace Zafkiel 
{

class FontLoader final : public AssetLoader
{
  public:
    virtual Ref<Asset> Load(const Ref<EditorAssetMetadata> &textureMetadata) override;
};

}