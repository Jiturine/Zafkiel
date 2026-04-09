#pragma once
#include "editor/Resource/AssetLoaders/AssetLoader.h"
#include "editor/Resource/EditorAsset.h"


namespace Zafkiel 
{
class SceneLoader final : public AssetLoader
{
  public:
    Ref<Asset> Load(const Ref<EditorAssetMetadata> &sceneMetadata);
};

}