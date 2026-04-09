#pragma once
#include "editor/Resource/EditorAsset.h"

namespace Zafkiel 
{

class AssetLoader
{
  public:
    virtual ~AssetLoader() = default;
    virtual Ref<Asset> Load(const Ref<EditorAssetMetadata> &metadata) = 0;
};

}