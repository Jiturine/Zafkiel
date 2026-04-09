#pragma once
#include "editor/Resource/EditorAsset.h"

namespace Zafkiel 
{

class MeshImporter
{
  public:
    Ref<EditorAssetMetadata> Import(const Path &assetPath)
    {
        AssetHandle meshHandle;
        auto meshMetadata = CreateRef<EditorAssetMetadata>(meshHandle, AssetType::Mesh, assetPath);
        return meshMetadata;
    }
};

}