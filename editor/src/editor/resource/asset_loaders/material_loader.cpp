#include "material_loader.h"
#include "core/meta/serializer/yaml_serializer.h"
#include "editor/resource/editor_asset_manager.h"

namespace Zafkiel 
{

Ref<Asset> MaterialLoader::Load(const Ref<EditorAssetMetadata> &materialMetadata)
{
    std::string str = FileSystem::ReadText(EditorAssetManager::GetAssetDirectory() / materialMetadata->filePath);
    auto materialAsset = CreateRef<MaterialAsset>(materialMetadata->handle, materialMetadata->detail.As<MaterialMetadata>().name, str);
    return materialAsset;
}

}