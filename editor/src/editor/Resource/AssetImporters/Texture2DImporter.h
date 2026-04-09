#pragma once
#include "editor/Resource/EditorAsset.h"

namespace Zafkiel 
{

class Texture2DImporter final
{
  public:
    Ref<EditorAssetMetadata> Import(const Path &assetPath, ImageFormat format)
    {
        AssetHandle handle;
        auto metadata = CreateRef<EditorAssetMetadata>(handle, AssetType::Texture2D, assetPath);
        metadata->detail.As<Texture2DMetadata>().format = format;
        return metadata;
    }
};

}