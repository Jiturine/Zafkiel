#include "editor/Resource/AssetLoaders/SceneLoader.h"
#include "editor/Resource/EditorAssetManager.h"

namespace Zafkiel 
{
Ref<Asset> SceneLoader::Load(const Ref<EditorAssetMetadata> &sceneMetadata)
{
    const std::string sceneData = FileSystem::ReadText(EditorAssetManager::Instance().GetAssetDirectory() / sceneMetadata->filePath);
    Ref<SceneAsset> sceneAsset = CreateRef<SceneAsset>(sceneMetadata->handle, sceneData);
    return sceneAsset;
}

}