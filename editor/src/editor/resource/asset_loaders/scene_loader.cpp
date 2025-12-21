#include "scene_loader.h"
#include "editor/resource/editor_asset_manager.h"

namespace Zafkiel 
{
Ref<Asset> SceneLoader::Load(const Ref<EditorAssetMetadata> &sceneMetadata)
{
    const std::string sceneData = FileSystem::ReadText(EditorAssetManager::GetAssetDirectory() / sceneMetadata->filePath);
    Ref<SceneAsset> sceneAsset = CreateRef<SceneAsset>(sceneMetadata->handle, sceneData);
    return sceneAsset;
}

}