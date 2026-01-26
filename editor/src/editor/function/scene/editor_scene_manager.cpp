#include "editor/function/scene/editor_scene_manager.h"
#include "editor/resource/editor_asset_manager.h"
#include "resource/scene_asset.h"

namespace Zafkiel 
{
Borrow<Scene> EditorSceneManager::OpenScene(const Path &filePath)
{
    AssetHandle sceneHandle;
    Path assetPath = filePath.RelativeTo(EditorAssetManager::GetAssetDirectory());
    if (!EditorAssetManager::IsFileRegisterd(assetPath))
    {
        sceneHandle = EditorAssetManager::ImportAsset(assetPath);
    }
    else
    {
        sceneHandle = EditorAssetManager::GetRegisterdAsset(assetPath);
    }
    auto sceneAsset = EditorAssetManager::GetAsset(sceneHandle).As<SceneAsset>();
    scenes.push_back(sceneAsset->LoadScene());
    activeSceneIndex = scenes.size() - 1;
    return Borrow(scenes.back());
}

}