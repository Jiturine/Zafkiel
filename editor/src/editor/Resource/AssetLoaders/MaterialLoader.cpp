#include "Core/Meta/Serializer/YamlSerializer.h"
#include "editor/Resource/AssetLoaders/MaterialLoader.h"
#include "editor/Resource/EditorAssetManager.h"

namespace Zafkiel 
{

Ref<Asset> MaterialLoader::Load(const Ref<EditorAssetMetadata> &materialMetadata)
{
    std::string str = FileSystem::ReadText(EditorAssetManager::Instance().GetAssetDirectory() / materialMetadata->filePath);
    auto materialAsset = CreateRef<MaterialAsset>(materialMetadata->handle, materialMetadata->detail.As<MaterialMetadata>().name, str);
    return materialAsset;
}

}