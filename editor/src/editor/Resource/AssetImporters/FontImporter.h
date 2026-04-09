#pragma once
#include "editor/Resource/EditorAsset.h"

namespace Zafkiel 
{

class FontImporter
{
  public:
    Ref<EditorAssetMetadata> Import(const Path &assetPath);
};

}