#include "asset_importer.h"

namespace Zafkiel
{

Ref<Asset> AssetImporter::ImportAsset(const AssetMetadata &metadata) const
{
    switch (metadata.type)
    {
        using enum AssetType;
    case Texture2D:
        return ImportTexture2D(metadata.filePath);

    default:
        Log::CoreError("Unknown data type!");
        break;
    }
    return nullptr;
}

Ref<Asset> AssetImporter::ImportTexture2D(const Path &path) const
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(1);
    auto data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
    if (!data)
    {
        Log::CoreError("Failed to load image!");
    }
    TextureSpecification spec;
    spec.width = width;
    spec.height = height;

    if (channels == 4)
    {
        spec.format = ImageFormat::RGB8;
    }
    else if (channels == 3)
    {
        spec.format = ImageFormat::RGBA8;
    }
    else
    {
        Log::CoreError("Format not supported!");
    }

    size_t size = width * height * channels;
    Ref<Asset> texture = context->CreateTexture2D(spec, Buffer(data, size));

    stbi_image_free(data);

    return texture;
}
}