#pragma once

#include "function/scene/scene.h"
#include "resource/asset.h"

namespace Zafkiel 
{
class SceneAsset : public Asset 
{
  public:
    virtual AssetType GetAssetType() const override { return AssetType::Scene; }

    SceneAsset(AssetHandle handle, const std::string &sceneData)
        : Asset(handle)
    {
        scene = CreateScope<Scene>(sceneData);
    }

    Observer<Scene> GetScene() { return scene; }

  private:
    Scope<Scene> scene;
};  
}