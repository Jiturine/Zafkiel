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
        : Asset(handle), sceneData(sceneData)
    {
    }
    
    Scope<Scene> LoadScene() 
    {
        return CreateScope<Scene>(sceneData);
    }

  private:
    std::string sceneData;
};  
}