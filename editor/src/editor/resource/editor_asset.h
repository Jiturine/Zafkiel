#pragma once

#include "resource/asset.h"
#include "platform/filesystem/filesystem.h"

namespace Zafkiel 
{

struct EditorAssetMetadata final : public AssetMetadata
{
    EditorAssetMetadata(AssetHandle handle, AssetType type, const std::vector<AssetHandle> &dependencies, const Path &assetPath) 
        : AssetMetadata(handle, type, dependencies), filePath(assetPath) {}
    EditorAssetMetadata(AssetHandle handle, AssetType type, const Path &assetPath) 
        : AssetMetadata(handle, type), filePath(assetPath) {}
    Path filePath;
};

}