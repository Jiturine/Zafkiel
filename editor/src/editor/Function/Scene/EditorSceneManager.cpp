#include "editor/Function/Scene/EditorSceneManager.h"
#include "editor/Resource/EditorAssetManager.h"
#include "Resource/SceneAsset.h"

namespace Zafkiel 
{
Scene &EditorSceneManager::OpenScene(const Path &filePath)
{
    AssetHandle sceneHandle;
    Path assetPath = filePath.RelativeTo(EditorAssetManager::Instance().GetAssetDirectory());
    if (!EditorAssetManager::Instance().IsFileRegisterd(assetPath))
    {
        sceneHandle = EditorAssetManager::Instance().ImportAsset(assetPath);
    }
    else
    {
        sceneHandle = EditorAssetManager::Instance().GetRegisterdAsset(assetPath);
    }
    auto sceneAsset = EditorAssetManager::Instance().GetAsset(sceneHandle).As<SceneAsset>();
    scenes.push_back(sceneAsset->LoadScene());
    activeSceneIndex = scenes.size() - 1;
    return *scenes.back().get();
}

}