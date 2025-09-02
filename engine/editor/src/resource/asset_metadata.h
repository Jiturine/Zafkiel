#pragma once
#include "resource/asset.h"
#include "platform/filesystem/filesystem.h"

namespace Zafkiel
{
struct AssetMetadata
{
    AssetType type;
    Path filePath;
};
}