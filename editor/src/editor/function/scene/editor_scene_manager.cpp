#include "editor_scene_manager.h"
#include "editor/resource/editor_asset_manager.h"
#include "resource/scene_asset.h"

namespace Zafkiel 
{
Observer<Scene> EditorSceneManager::OpenSceneImpl(const Path &filePath)
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
    activeScene = sceneAsset->GetScene();
    return activeScene;
}

}