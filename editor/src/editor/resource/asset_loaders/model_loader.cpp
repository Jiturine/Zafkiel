#include "editor/resource/asset_loaders/model_loader.h"
#include "editor/resource/editor_asset_manager.h"

namespace Zafkiel 
{

Ref<Asset> ModelLoader::Load(const Ref<EditorAssetMetadata> &metadata)
{
    std::string modelStr = FileSystem::ReadText(EditorAssetManager::GetAssetDirectory() / metadata->filePath);
    Ref<ModelAsset> modelAsset = CreateRef<ModelAsset>(metadata->handle, modelStr);
    return modelAsset;
}

}
