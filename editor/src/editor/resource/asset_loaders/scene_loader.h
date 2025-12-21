#pragma once
#include "editor/resource/asset_loaders/asset_loader.h"
#include "editor/resource/editor_asset.h"


namespace Zafkiel 
{
class SceneLoader final : public AssetLoader
{
  public:
    Ref<Asset> Load(const Ref<EditorAssetMetadata> &sceneMetadata);
};

}