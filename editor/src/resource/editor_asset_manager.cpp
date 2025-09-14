#include "editor_asset_manager.h"
#include "core/meta/serializer/deserialize.h"
#include "stb_image.h"
#include "function/engine.h"
#include "platform/filesystem/filesystem.h"

namespace Zafkiel
{
EditorAssetManager::EditorAssetManager() {}

bool EditorAssetManager::IsAssetValid(AssetHandle handle) const
{
    return assetRegistry.contains(handle);
}
bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
{
    return loadedAssets.contains(handle);
}

Ref<Asset> EditorAssetManager::GetAsset(AssetHandle handle)
{
    if (!IsAssetValid(handle)) return nullptr;

    Ref<Asset> asset;
    if (IsAssetLoaded(handle))
    {
        asset = loadedAssets.at(handle);
    }
    else
    {
        const AssetMetadata &metadata = assetRegistry.at(handle);
        asset = LoadAsset(metadata);
        if (!asset)
        {
            Log::CoreError("Asset import failed!");
        }
        loadedAssets[handle] = asset;
    }
    return asset;
}

void EditorAssetManager::SetAssetDirectory(const Path &path)
{
    assetDirectory = path;
}

Path EditorAssetManager::GetAssetDirectory()
{
    return assetDirectory;
}
void EditorAssetManager::SerializeAssetRegistry()
{
    const auto &str = Serialize(assetRegistry);
    Log::CoreTrace("{}", str);
}

void EditorAssetManager::DeserializeAssetRegistry()
{
    const std::string str = FileSystem::ReadText(assetDirectory / "asset_registry.yaml");
    assetRegistry = Deserialize<std::unordered_map<AssetHandle, AssetMetadata>>(str);
}

AssetHandle EditorAssetManager::ImportAsset(const Path &assetPath)
{
    AssetHandle handle;
    if (assetPath.extension().string() == ".png")
    {
        assetRegistry[handle] = AssetMetadata{AssetType::Texture2D, assetDirectory / assetPath};
    }
    else
    {
        Log::CoreError("Unknown asset type!");
    }
    return handle;
}

Ref<Asset> EditorAssetManager::LoadAsset(const AssetMetadata &metadata) const
{
    switch (metadata.type)
    {
        using enum AssetType;
    case Texture2D:
        return LoadTexture2D(metadata.filePath);

    default:
        Log::CoreError("Unknown data type!");
        break;
    }
    return nullptr;
}

Ref<Asset> EditorAssetManager::LoadTexture2D(const Path &path) const
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(1);
    auto data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
    if (!data)
    {
        Log::CoreError("Failed to load image! Path: {}", path.string());
    }
    TextureSpecification spec;
    spec.width = width;
    spec.height = height;

    if (channels == 4)
    {
        spec.format = ImageFormat::RGBA8;
    }
    else if (channels == 3)
    {
        spec.format = ImageFormat::RGB8;
    }
    else
    {
        Log::CoreError("Format not supported!");
    }

    size_t size = width * height * channels;
    Ref<Asset> texture = Engine::GetGraphicsContext()->CreateTexture2D(spec, Buffer(data, size));

    stbi_image_free(data);

    return texture;
}

}