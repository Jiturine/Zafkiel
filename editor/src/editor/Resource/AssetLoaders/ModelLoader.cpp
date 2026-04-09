#include "editor/Resource/AssetLoaders/ModelLoader.h"
#include "editor/Resource/EditorAssetManager.h"

namespace Zafkiel 
{

Ref<Asset> ModelLoader::Load(const Ref<EditorAssetMetadata> &metadata)
{
    std::string modelStr = FileSystem::ReadText(EditorAssetManager::Instance().GetAssetDirectory() / metadata->filePath);
    Ref<ModelAsset> modelAsset = CreateRef<ModelAsset>(metadata->handle, modelStr);
    return modelAsset;
}

}
