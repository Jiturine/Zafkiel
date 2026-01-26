#include "editor/resource/asset_loaders/texture2d_loader.h"
#include <stb_image.h>
#include "editor/resource/editor_asset_manager.h"

namespace Zafkiel 
{

Ref<Asset> Texture2DLoader::Load(const Ref<EditorAssetMetadata> &textureMetadata)
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(1);
    Path texturePath = EditorAssetManager::GetAssetDirectory() / textureMetadata->filePath;
    auto data = stbi_load(texturePath.string().c_str(), &width, &height, &channels, 4);
    if (!data)
    {
        Log::Error("Failed to load image! Path: {}", texturePath.string());
    }
    Texture2DSpecification spec
    {
        .width = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
        .format = ImageFormat::RGBA8,
        .usages = { ImageUsage::Upload, ImageUsage::Sampled },
        .updateFrequency = ImageUpdateFrequency::Static,
        .wrap = TextureWrap::Repeat,
        .filter = TextureFilter::Nearest,
        .samples = 1,
    };

    size_t size = width * height * 4;
    ScopedBuffer buffer(data, size);
    Ref<Texture2DAsset> texture = CreateRef<Texture2DAsset>(textureMetadata->handle, spec, std::move(buffer));

    stbi_image_free(data);

    return texture;
}


}
