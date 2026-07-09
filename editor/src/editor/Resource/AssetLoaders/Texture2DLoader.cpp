#include "editor/Resource/AssetLoaders/Texture2DLoader.h"
#include "editor/Resource/EditorAssetManager.h"
#include <stb_image.h>

namespace Zafkiel 
{

Ref<Asset> Texture2DLoader::Load(const Ref<EditorAssetMetadata> &textureMetadata)
{
    int width, height, channels;
    Path texturePath = EditorAssetManager::Instance().GetAssetDirectory() / textureMetadata->filePath;

    auto format = textureMetadata->detail.As<Texture2DMetadata>().format;
    auto desiredChannels = ImageFormatToChannels(format);
    auto data = stbi_load(texturePath.string().c_str(), &width, &height, &channels, desiredChannels);
    if (!data)
    {
        Log::Error("Failed to load image! Path: {}", texturePath.string());
    }
    Texture2DDesc desc
    {
        .width = static_cast<uint32>(width),
        .height = static_cast<uint32>(height),
        .format = format,
    };

    size_t size = width * height * desiredChannels;
    ScopedBuffer buffer(data, size);
    Ref<Texture2DAsset> texture = CreateRef<Texture2DAsset>(textureMetadata->handle, desc, MoveTemp(buffer));

    stbi_image_free(data);

    return texture;
}

}
