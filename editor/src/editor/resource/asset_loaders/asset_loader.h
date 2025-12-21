#pragma once
#include "editor/resource/editor_asset.h"

namespace Zafkiel 
{

class AssetLoader
{
  public:
    virtual ~AssetLoader() = default;
    virtual Ref<Asset> Load(const Ref<EditorAssetMetadata> &metadata) = 0;
};

}