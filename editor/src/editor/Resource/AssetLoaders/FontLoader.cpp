#include "editor/Resource/AssetLoaders/FontLoader.h"
#include "editor/Resource/EditorAssetManager.h"
#include "Resource/FontAsset.h"

namespace Zafkiel
{

Ref<Asset> FontLoader::Load(const Ref<EditorAssetMetadata> &fontMetadata) 
{
    std::string str = FileSystem::ReadText(EditorAssetManager::Instance().GetAssetDirectory() / fontMetadata->filePath);
    auto fontAsset = CreateRef<FontAsset>(fontMetadata->handle);
    return fontAsset;
}

}