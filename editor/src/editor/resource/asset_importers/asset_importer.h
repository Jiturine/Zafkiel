#pragma once
#include "editor/resource/editor_asset.h"

namespace Zafkiel 
{

class AssetImporter
{
  public:
    virtual ~AssetImporter() = default;
    virtual Ref<EditorAssetMetadata> Import(const Path &assetPath) = 0;
};

}